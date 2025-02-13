#include <Core/base_engine.hpp>
#include <Core/engine_loading_controllers.hpp>
#include <Core/keyboard.hpp>
#include <Core/logger.hpp>
#include <Core/reflection/class.hpp>
#include <Core/reflection/enum.hpp>
#include <Core/threading.hpp>
#include <Event/event.hpp>
#include <Event/event_data.hpp>
#include <Graphics/render_viewport.hpp>
#include <Graphics/rhi.hpp>
#include <Graphics/scene_render_targets.hpp>
#include <ScriptEngine/registrar.hpp>
#include <ScriptEngine/script_engine.hpp>
#include <ScriptEngine/script_function.hpp>
#include <Systems/engine_system.hpp>
#include <Systems/event_system.hpp>
#include <Systems/game_controller_system.hpp>
#include <Systems/keyboard_system.hpp>
#include <Systems/mouse_system.hpp>
#include <Systems/touchscreen_system.hpp>
#include <Window/config.hpp>
#include <Window/window.hpp>
#include <Window/window_manager.hpp>
#include <angelscript.h>


namespace Engine
{
	// Basic callbacks

	void EventSystem::on_window_close(const Event& event, bool is_quit)
	{
		WindowManager* manager = WindowManager::instance();
		Window* window         = manager->find(event.window_id());

		if (manager->main_window() == window || is_quit)
		{
			engine_instance->request_exit();
		}
		else if (window)
		{
			m_windows_to_destroy.push_back(window->id());
		}
	}

	static void on_resize(const Event& event)
	{
		WindowManager* manager = WindowManager::instance();
		Window* window         = manager->find(event.window_id());
		if (!window)
			return;

		const WindowEvent& window_event = event.get<const WindowEvent&>();

		{
			auto x                                = window_event.x;
			auto y                                = window_event.y;
			WindowRenderViewport* render_viewport = window->render_viewport();
			if (render_viewport)
			{
				render_viewport->on_resize({x, y});
			}
		}
	}

	static void on_orientation_changed(const Event& event)
	{
		WindowManager* manager = WindowManager::instance();
		Window* window         = manager->find(event.window_id());
		if (!window)
			return;

		const DisplayOrientationChangedEvent& display_event = event.get<const DisplayOrientationChangedEvent&>();
		{
			WindowRenderViewport* render_viewport = window->render_viewport();
			if (render_viewport)
			{
				render_viewport->on_orientation_changed(display_event.orientation);
			}
		}
	}

	EventSystem::EventSystem()
	{}

	const EventSystem::ListenerMap& EventSystem::listeners() const
	{
		return m_listeners;
	}

	EventSystemListenerID EventSystem::add_listener(EventType type, const Listener& listener)
	{
		return EventSystemListenerID(type, m_listeners[static_cast<Identifier>(type)].push(listener));
	}

	EventSystem& EventSystem::remove_listener(const EventSystemListenerID& id)
	{
		if (m_is_in_events_pooling)
			m_listeners_to_remove.push_back(id);
		else
			m_listeners[static_cast<EnumerateType>(id.m_type)].remove(id.m_id);

		return *this;
	}

	EventSystem& EventSystem::create()
	{
		Super::create();

		m_is_in_events_pooling = false;
		System::new_system<EngineSystem>()->register_subsystem(this);
		add_listener(EventType::Quit, std::bind(&EventSystem::on_window_close, this, std::placeholders::_1, true));
		add_listener(EventType::WindowClose, std::bind(&EventSystem::on_window_close, this, std::placeholders::_1, false));
		add_listener(EventType::WindowResized, on_resize);
		add_listener(EventType::DisplayOrientationChanged, on_orientation_changed);

		// Register subsystems
		new_system<KeyboardSystem>();
		new_system<MouseSystem>();
		new_system<TouchScreenSystem>();
		new_system<GameControllerSystem>();

		process_event_method(ProcessEventMethod::PoolEvents);

		return *this;
	}

	EventSystem& EventSystem::update(float dt)
	{
		Super::update(dt);
		(this->*m_process_events)();

		if (!m_windows_to_destroy.empty())
		{
			WindowManager* manager = WindowManager::instance();

			if (manager)
			{
				for (Identifier id : m_windows_to_destroy)
				{
					if (auto window = manager->find(id))
					{
						manager->destroy_window(window);
					}
				}
			}

			m_windows_to_destroy.clear();
		}
		return *this;
	}

	EventSystem& EventSystem::push_event(const Event& event)
	{
		bool is_nested = m_is_in_events_pooling;

		m_is_in_events_pooling = true;
		auto it                = m_listeners.find(static_cast<EnumerateType>(event.type()));
		if (it != m_listeners.end())
		{
			it->second.trigger(event);
		}

		it = m_listeners.find(static_cast<EnumerateType>(EventType::Undefined));

		if (it != m_listeners.end())
		{
			it->second.trigger(event);
		}

		m_is_in_events_pooling = is_nested;

		if (!m_listeners_to_remove.empty())
		{
			for (auto& id : m_listeners_to_remove)
			{
				remove_listener(id);
			}
			m_listeners_to_remove.clear();
		}

		return *this;
	}

	EventSystem& EventSystem::shutdown()
	{
		Super::shutdown();
		m_listeners.clear();
		return *this;
	}

	Name EventSystem::event_name(EventType type)
	{
		static Refl::Enum* event_type_enum = Refl::Enum::static_find("Engine::EventType", Refl::FindFlags::IsRequired);
		auto entry                         = event_type_enum->entry(static_cast<EnumerateType>(type));

		if (entry)
		{
			return entry->name;
		}

		return Name::undefined;
	}

	static void push_event_internal(const Event& event, void* self)
	{
		reinterpret_cast<EventSystem*>(self)->push_event(event);
	}

	EventSystem& EventSystem::wait_events()
	{
		WindowManager::instance()->wait_for_events(push_event_internal, this);
		return *this;
	}

	EventSystem& EventSystem::pool_events()
	{
		WindowManager::instance()->pool_events(push_event_internal, this);
		return *this;
	}

	EventSystem& EventSystem::process_event_method(ProcessEventMethod method)
	{
		if (method == ProcessEventMethod::PoolEvents)
		{
			m_process_events = &EventSystem::pool_events;
		}
		else
		{
			m_process_events = &EventSystem::wait_events;
		}
		return *this;
	}

	implement_engine_class(EventSystem, Refl::Class::IsScriptable)
	{}
}// namespace Engine
