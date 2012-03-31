
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <libusb-1.0/libusb.h>

#include "font_6x8.h"

#define VENDOR_ID 0x04d8
#define PRODUCT_ID 0xc002

#define OUT_REPORT_LED_STATE            0x81
#define OUT_REPORT_LCD_BACKLIGHT        0x91
#define OUT_REPORT_LCD_CONTRAST         0x92

#define OUT_REPORT_CMD                  0x94
#define OUT_REPORT_DATA                 0x95
#define OUT_REPORT_CMD_DATA             0x96

#define SCREEN_H                        64
#define SCREEN_W                        256

typedef struct Command {
    void (*cb) (unsigned char *data, int size);
    unsigned char data[64];
    int size;
} Command;

struct Command queue[1024];

int queueEnd = 0;
int queueStart = 0;

unsigned char locked = 0;

static unsigned char pLG_framebuffer[256*64];

/* used to display white text on black background or inverse */
unsigned char inverted = 0;

static struct libusb_device_handle *devh = NULL;
static struct libusb_transfer *lcd_transfer = NULL;

static int do_exit = 0;

static pthread_t poll_thread;
static pthread_t command_thread;
static pthread_cond_t exit_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t exit_cond_lock = PTHREAD_MUTEX_INITIALIZER;

static void queue_push(void (*cb), unsigned char *data, int size) {
    int i;
    if (queueEnd >= 1024)
        return;
    queue[queueEnd].cb = cb;
    queue[queueEnd].size = size;
    for(i = 0; i < size; i++) {
        queue[queueEnd].data[i] = data[i];
    }
    printf("Data0: %x, %x, %x\n", queue[queueEnd].data[0], queue[queueEnd].data[1], queue[queueEnd].data[2]);
    queueEnd++;
}

struct Command * queue_pop(void) {
    if( queueStart >= queueEnd )
        return NULL;
    return &queue[queueStart++];
}

static void request_exit(int code) {
    do_exit = code;
    pthread_cond_signal(&exit_cond);
}

static void *command_thread_main(void *arg) {
    struct Command *cmd;
    while (!do_exit) {
        if(locked) continue;
        locked = 1;
        cmd = queue_pop();
        if(cmd != NULL) {
            cmd->cb(cmd->data, cmd->size);
            printf("Data1: %x, %x, %x, %d\n", cmd->data[0], cmd->data[1], cmd->data[2], cmd->size);
        }
    }
    return NULL;
}

static void *poll_thread_main(void *arg) {
    int r = 0;
    while ( !do_exit) {
        struct timeval tv = { 1, 0};
        r = libusb_handle_events_timeout(NULL, &tv);
        if( r < 0 ) {
            request_exit(2);
            break;
        }
    }
    return NULL;
}

void cb_lcd(struct libusb_transfer *transfer) {
    if(transfer->status != LIBUSB_TRANSFER_COMPLETED) {
        fprintf(stderr, "transfer not completed!\n");
    }
    printf("cb_lcd: length=%d, actual_length=%d\n", transfer->length, transfer->actual_length);
    locked = 0;
}

void drv_pLG_real_send(unsigned char *data, int size) {
    libusb_fill_interrupt_transfer(lcd_transfer, devh, LIBUSB_ENDPOINT_OUT + 1, data, 
	size, cb_lcd, NULL, 0);
    libusb_submit_transfer(lcd_transfer);
}

void drv_pLG_send(unsigned char *data, int size) {
    queue_push(drv_pLG_real_send, data, size);
}

static void drv_pLG_update_img() {
    unsigned char cmd3[64] = { OUT_REPORT_CMD_DATA };   /* send command + data */
    unsigned char cmd4[64] = { OUT_REPORT_DATA };       /* send data only */

    int index, bit, x, y;
    unsigned char cs, line;
    unsigned char pixel;

    for (cs = 0; cs < 4; cs++) {
        unsigned char chipsel = (cs << 2);      //chipselect
        for (line = 0; line < 8; line++) {

            cmd3[0] = OUT_REPORT_CMD_DATA;
            cmd3[1] = chipsel;
            cmd3[2] = 0x02;
            cmd3[3] = 0x00;
            cmd3[4] = 0x00;
            cmd3[5] = 0xb8 | line;
            cmd3[6] = 0x00;
            cmd3[7] = 0x00;
            cmd3[8] = 0x40;
            cmd3[9] = 0x00;
            cmd3[10] = 0x00;
            cmd3[11] = 32;


            cmd4[0] = OUT_REPORT_DATA;
            cmd4[1] = chipsel | 0x01;
            cmd4[2] = 0x00;
            cmd4[3] = 0x00;
            cmd4[4] = 32;

            for (index = 0; index < 32; index++) {
                pixel = 0x00;

                for (bit = 0; bit < 8; bit++) {
                    x = cs * 64 + index;
                    y = (line * 8 + bit + 0) % SCREEN_H;

                    if (pLG_framebuffer[y * 256 + x] ^ inverted)
                        pixel |= (1 << bit);
                }
                cmd3[12 + index] = pixel;
            }

            for (index = 32; index < 64; index++) {
                pixel = 0x00;

                for (bit = 0; bit < 8; bit++) {
                    x = cs * 64 + index;
                    y = (line * 8 + bit + 0) % SCREEN_H;
                    if (pLG_framebuffer[y * 256 + x] ^ inverted)
                        pixel |= (1 << bit);
                }

                cmd4[5 + (index - 32)] = pixel;
            }

            drv_pLG_send(cmd3, 44);
            drv_pLG_send(cmd4, 38);
        }
    }

}

void drv_pLG_clear(void)
{
    unsigned char cmd[3] = { 0x93, 0x01, 0x00 };        /* init display */
    unsigned char cmd2[9] = { OUT_REPORT_CMD }; /* init display */
    unsigned char cmd3[64] = { OUT_REPORT_CMD_DATA };   /* clear screen */
    unsigned char cmd4[64] = { OUT_REPORT_CMD_DATA };   /* clear screen */

    int init, index;
    unsigned char cs, line;

    drv_pLG_send(cmd, 3);

    for (init = 0; init < 4; init++) {
        unsigned char cs = ((init << 2) & 0xFF);

        cmd2[0] = OUT_REPORT_CMD;
        cmd2[1] = cs;
        cmd2[2] = 0x02;
        cmd2[3] = 0x00;
        cmd2[4] = 0x64;
        cmd2[5] = 0x3F;
        cmd2[6] = 0x00;
        cmd2[7] = 0x64;
        cmd2[8] = 0xC0;

        drv_pLG_send(cmd2, 9);
    }


    for (cs = 0; cs < 4; cs++) {
        unsigned char chipsel = (cs << 2);      //chipselect
        for (line = 0; line < 8; line++) {
            cmd3[0] = OUT_REPORT_CMD_DATA;
            cmd3[1] = chipsel;
            cmd3[2] = 0x02;
            cmd3[3] = 0x00;
            cmd3[4] = 0x00;
            cmd3[5] = 0xb8 | line;
            cmd3[6] = 0x00;
            cmd3[7] = 0x00;
            cmd3[8] = 0x40;
            cmd3[9] = 0x00;
            cmd3[10] = 0x00;
            cmd3[11] = 32;

            unsigned char temp = 0;

            for (index = 0; index < 32; index++) {
                cmd3[12 + index] = temp;
            }

            drv_pLG_send(cmd3, 44);

            cmd4[0] = OUT_REPORT_DATA;
            cmd4[1] = chipsel | 0x01;
            cmd4[2] = 0x00;
            cmd4[3] = 0x00;
            cmd4[4] = 32;

            for (index = 32; index < 64; index++) {
                temp = 0x00;
                cmd4[5 + (index - 32)] = temp;
            }
            drv_pLG_send(cmd4, 38);
        }
    }
}


static int find_device(void)
{
    devh = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    return devh ? 0 : -EIO;
}


static void sighandler(int signum)
{
    request_exit(1);
}

static void fill_pixels(void)
{
    int i, r, c, row, col;
    char *text = "picoLCDGraphics ";
    char ch;
    memset(pLG_framebuffer, 0, sizeof(pLG_framebuffer));
    for( i = 0; i < 256/6*8; i++) {
        r = i / 42 * 8;
        c = i % 42 * 6;
        ch = text[i % strlen(text)];
        for( row = 0; row < 8; row++ ) {
            for( col = 0; col < 6; col++ ) {
                if (Font_6x8[(int)ch][row] & (1 << (5-col)) )
			pLG_framebuffer[(r + row) * 256 + c + col] = 1;
            }
        }
    }
}

int main(void)
{
    struct sigaction sigact;
    int r = 1;

    r = libusb_init(NULL);
    if( r < 0) {
        fprintf(stderr, "failed to initialize libusb\n");
        exit(1);
    }

    r = find_device();
    if(r < 0) {
        fprintf(stderr, "Could not find device\n");
        goto out;
    }

    r = libusb_claim_interface(devh, 0);
    if (r < 0) {
        fprintf(stderr, "usb_claim_interface error %d\n", r);
        goto out;
    }

    printf("claimed interface\n");

    lcd_transfer = libusb_alloc_transfer(0);

    if(!lcd_transfer) {
        fprintf(stderr, "Unable to allocate transfer\n");
        goto out;
    }

    drv_pLG_clear();
    fill_pixels();
    drv_pLG_update_img();

    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGTERM, &sigact, NULL);
    sigaction(SIGQUIT, &sigact, NULL);


    r = pthread_create(&poll_thread, NULL, poll_thread_main, NULL);

    if (r)
        goto out_deinit;

    r = pthread_create(&command_thread, NULL, command_thread_main, NULL);

    if (r)
        goto out_deinit;

    while (!do_exit) {
        pthread_mutex_lock(&exit_cond_lock);
        pthread_cond_wait(&exit_cond, &exit_cond_lock);
        pthread_mutex_unlock(&exit_cond_lock);
    }

    pthread_join(poll_thread, NULL);

    pthread_join(command_thread, NULL);

    if (lcd_transfer) {
        r = libusb_cancel_transfer(lcd_transfer);
        if (r < 0)
            goto out_deinit;
    }

    while (lcd_transfer)
        if (libusb_handle_events(NULL) < 0)
            break;
    
    
out_deinit:
    libusb_free_transfer(lcd_transfer);
out_release:
    libusb_release_interface(devh, 0);
out:
    libusb_close(devh);
    libusb_exit(NULL);
    return r >= 0 ? r : -r;    
}
