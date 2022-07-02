#include "vlib.hpp"

#include <exception>

namespace vlib {

	void ThrowException(const bool& compare, const char* source) {

#ifdef VLIB_THROW_EXCEPTIONS
		if (compare == true) {

			throw std::exception(source);
		}
#endif
	}

	VProcess::VProcess(const ProcessInfo& info) {
		
		auto wnd_proc = [](HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) -> LRESULT {

			switch (msg) {

			case WM_SYSCOMMAND: {

				if ((w_param & 0xfff0) == SC_KEYMENU) {

					return 0;
				}
				else {

					break;
				}
			}
			case WM_DESTROY: {

				PostQuitMessage(0);

				return 0;
			}
			case WM_PAINT: {

				static const auto background_color = CreateSolidBrush(RGB(12, 12, 12));

				PAINTSTRUCT ps{};
				auto hdc = BeginPaint(hwnd, &ps);
				
				FillRect(hdc, &ps.rcPaint, background_color);

				EndPaint(hwnd, &ps);

				return 0;
			}
			}

			if (const auto return_value = WndProc(hwnd, msg, w_param, l_param); return_value != -1) {

				return return_value;
			}

			return DefWindowProc(hwnd, msg, w_param, l_param);
		};
		try {

			this->p_class_info = {

				.cbSize{ sizeof(WNDCLASSEX) },
				.style{ CS_HREDRAW | CS_VREDRAW },
				.lpfnWndProc{ wnd_proc },
				.cbClsExtra{ 0 },
				.cbWndExtra{ 0 },
				.hInstance{ info.m_instance },
				.hIcon{ LoadIcon(info.m_instance, MAKEINTRESOURCE(info.m_icon)) },
				.hCursor{ LoadCursor(info.m_instance, IDC_ARROW) },
				.hbrBackground{ CreateSolidBrush(RGB(0x80,0x0,0xFF)) },
				.lpszMenuName{ nullptr },
				.lpszClassName{ info.m_class_name },
				.hIconSm{ LoadIcon(info.m_instance, MAKEINTRESOURCE(info.m_icon)) }
			};

			ThrowException(RegisterClassEx(&this->p_class_info) == 0, "RegisterClassEx");

			this->p_window_handle = CreateWindowEx(info.m_extended_flags, info.m_class_name, info.m_window_name, info.m_flags, info.m_window_rect.m_x, info.m_window_rect.m_y, info.m_window_rect.m_w, info.m_window_rect.m_h, nullptr, nullptr, info.m_instance, nullptr);

			ThrowException(this->p_window_handle == nullptr, "CreateWindowEx");

			SetLayeredWindowAttributes(this->p_window_handle, RGB(0x80, 0x0, 0xFF), 0, LWA_COLORKEY);
		}
		catch (const std::exception& ex) {

			MessageBox(nullptr, std::system_category().message(GetLastError()).c_str(), ex.what(), MB_OK);
		}
	}
}
