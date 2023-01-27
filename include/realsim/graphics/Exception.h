#pragma once
#include <exception>
#include <intsafe.h>
#include <stdio.h>

namespace RSim::Graphics
{
    // Simple class for handling HRESULT's , check https://github.com/Microsoft/DirectXTK/wiki/ThrowIfFailed for more information.
    class ComException : public std::exception
    {
    public:
        ComException(HRESULT hr) : result(hr) {}

        const char* what() const noexcept override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X",
                static_cast<unsigned int>(result));
            return s_str;
        }
    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw ComException(hr);
        }
    }

}
