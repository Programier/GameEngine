#include <Core/engine.hpp>
#include <Window/monitor.hpp>
#include <Window/window.hpp>

namespace Engine
{
    // Monitor init
    namespace Monitor
    {
        static MonitorInfo monitor_info;

        ENGINE_EXPORT void update()
        {
            Window* window = engine_instance->window();
            if (window)
            {
                window->update_monitor_info(monitor_info);
            }
        }

        ENGINE_EXPORT uint_t height()
        {
            return monitor_info.height;
        }

        ENGINE_EXPORT uint_t width()
        {
            return monitor_info.width;
        }

        ENGINE_EXPORT int_t refresh_rate()
        {
            return monitor_info.refresh_rate;
        }

        ENGINE_EXPORT Size2D size()
        {
            return {static_cast<float>(monitor_info.width), static_cast<float>(monitor_info.height)};
        }

        ENGINE_EXPORT const DPI& dpi()
        {
            return monitor_info.dpi;
        }

        ENGINE_EXPORT const MonitorInfo& info()
        {
            return monitor_info;
        }

        ENGINE_EXPORT Size2D physical_size(PhysicalSizeMetric metric)
        {
            Size2D inches = size();
            inches.x /= monitor_info.dpi.hdpi;
            inches.y /= monitor_info.dpi.vdpi;

            if (metric == PhysicalSizeMetric::Сentimeters)
            {
                inches *= 2.54f;
            }

            return inches;
        }
    }// namespace Monitor
}// namespace Engine
