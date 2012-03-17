#ifndef spANativeWindow_h

#include <android/native_window.h>

namespace WebCore {

class spANativeWindow
{
public:
    
    inline spANativeWindow() : m_ptr(0) { }

    spANativeWindow(ANativeWindow* other);
    spANativeWindow(const spANativeWindow& other);

    ~spANativeWindow();
    
    // Assignment

    spANativeWindow& operator = (ANativeWindow* other);
    spANativeWindow& operator = (const spANativeWindow& other);
    
  
    // Reset
    
    void clear();
    
    // Accessors

    // inline  ANativeWindow&      operator* () const  { return *m_ptr; }
    inline  ANativeWindow*      operator-> () const { return m_ptr;  }
    inline  ANativeWindow*      get() const         { return m_ptr; }

    // Operators
        
    inline bool operator == (const spANativeWindow& o) const {
      return m_ptr == o.m_ptr;
    }

    inline bool operator != (const spANativeWindow& o) const {
      return m_ptr != o.m_ptr;
    }

private:    
    ANativeWindow*              m_ptr;
};

}

#endif // spANativeWindow_h
