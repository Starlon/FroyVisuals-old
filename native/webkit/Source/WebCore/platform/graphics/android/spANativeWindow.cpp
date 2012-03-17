#include "spANativeWindow.h"

namespace WebCore {

spANativeWindow::spANativeWindow(ANativeWindow* other)
    : m_ptr(other)
{
    if (other) ANativeWindow_acquire(other);
}

spANativeWindow::spANativeWindow(const spANativeWindow& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr) ANativeWindow_acquire(m_ptr);
}

spANativeWindow::~spANativeWindow()
{
    if (m_ptr) ANativeWindow_release(m_ptr);
}

spANativeWindow& spANativeWindow::operator = (const spANativeWindow& other) {
    ANativeWindow* otherPtr(other.m_ptr);
    if (otherPtr) ANativeWindow_acquire(otherPtr);
    if (m_ptr) ANativeWindow_release(m_ptr);
    m_ptr = otherPtr;
    return *this;
}

spANativeWindow& spANativeWindow::operator = (ANativeWindow* other)
{
    if (other) ANativeWindow_acquire(other);
    if (m_ptr) ANativeWindow_release(m_ptr);
    m_ptr = other;
    return *this;
}

void spANativeWindow::clear()
{
    if (m_ptr) {
        ANativeWindow_release(m_ptr);
        m_ptr = 0;
    }
}

} // namespace WebCore
