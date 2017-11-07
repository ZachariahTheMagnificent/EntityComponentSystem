#include <iostream>
#include <type_traits>
#include <string>

template<typename Base, typename Component>
class ComponentList : public Base
{
public:
	ComponentList ( ) = default;
	virtual ~ComponentList ( ) = default;

protected:
	template<typename NextComponent>
	using Add = ComponentList<ComponentList, NextComponent>;

	virtual Component* Get ( const Component*const dummy_pointer ) = 0;
	virtual const Component* Get ( const Component*const dummy_pointer ) const = 0;
};

template<typename Component>
class ComponentList<void, Component>
{
public:
	ComponentList ( ) = default;
	virtual ~ComponentList ( ) = default;

protected:
	template<typename NextComponent>
	using Add = ComponentList<ComponentList, NextComponent>;

	virtual Component* Get ( const Component*const dummy_pointer ) = 0;
	virtual const Component* Get ( const Component*const dummy_pointer ) const = 0;
};

template<typename List, typename...Components>
class BaseEntity_;

template<typename Base, typename CurrentComponent, typename Component, typename...OtherComponents>
class BaseEntity_<ComponentList<Base, CurrentComponent>, Component, OtherComponents...> : public BaseEntity_<typename ComponentList<Base, CurrentComponent>::Add<Component>, OtherComponents...>
{
public:
	BaseEntity_ ( ) = default;
	virtual ~BaseEntity_ ( ) = default;
};

template<typename List>
class BaseEntity_<List> : public List
{
public:
	BaseEntity_ ( ) = default;
	virtual ~BaseEntity_ ( ) = default;

	template<typename Component>
	Component* GetComponent ( )
	{
		const Component*const component = nullptr;
		return Get ( component );
	}
	template<typename Component>
	const Component* GetComponent ( ) const
	{
		const Component*const component = nullptr;
		return Get ( component );
	}
};

template<typename FirstComponent, typename...OtherComponents>
using BaseEntity = BaseEntity_<ComponentList<void, FirstComponent>, OtherComponents...>;

template<typename Base, typename Component>
class Dispatcher : public Base
{
public:
	Dispatcher ( ) = default;
	virtual ~Dispatcher ( ) = default;

protected:
	template<typename Component>
	using Add = Dispatcher<Dispatcher, Component>;

	template<typename Implementation, bool Convertible = std::is_convertible<Implementation, Component&>::value>
	struct Dispatch
	{
		static Component* Execute ( Implementation& implementation )
		{
			return &static_cast< Component& >( implementation );
		}
		static const Component* Execute ( const Implementation& implementation )
		{
			return &static_cast< const Component& >( implementation );
		}
	};
	template<typename Implementation>
	struct Dispatch<Implementation, false>
	{
		static Component* Execute ( const Implementation& )
		{
			return nullptr;
		}
	};

	Component* Get ( const Component*const ) override
	{
		auto& implementation = GetImplementation ( );
		using ImplementationType = std::decay_t<decltype ( implementation )>;

		return Dispatch<ImplementationType>::Execute ( implementation );
	}
	const Component* Get ( const Component*const ) const override
	{
		const auto& implementation = GetImplementation ( );
		using ImplementationType = decltype ( implementation );

		return Dispatch<ImplementationType>::Execute ( implementation );
	}
};

template<typename Base, typename...Components>
class Entity_;

template<typename Base, typename FirstComponent, typename...OtherComponents>
class Entity_<Base, FirstComponent, OtherComponents...> : public Entity_<Dispatcher<Base, FirstComponent>, OtherComponents...>
{
public:
	Entity_ ( ) = default;
	virtual ~Entity_ ( ) = default;
};

template<typename Base, typename CurrentComponent, typename Component, typename...OtherComponents>
class Entity_<Dispatcher<Base, CurrentComponent>, Component, OtherComponents...> : public Entity_<Dispatcher<Base, CurrentComponent>::Add<Component>, OtherComponents...>
{
public:
	Entity_ ( ) = default;
	virtual ~Entity_ ( ) = default;
};

template<typename Base>
class Entity_<Base> : public Base
{
public:
	Entity_ ( ) = default;
	virtual ~Entity_ ( ) = default;
};

template<typename Implementation, typename FirstComponent, typename...OtherComponents>
class Implementor : public BaseEntity<FirstComponent, OtherComponents...>
{
public:
	using Base = BaseEntity<FirstComponent, OtherComponents...>;

	Implementor ( ) = default;
	virtual ~Implementor ( ) = default;

	Implementation& GetImplementation ( )
	{
		return implementation_;
	}
	const Implementation& GetImplementation ( ) const
	{
		return implementation_;
	}

private:
	Implementation implementation_;
};

template<typename Implementation, typename...Components>
using Entity = Entity_<Implementor<Implementation, Components...>, Components...>;

template<typename FirstComponent, typename...OtherComponents>
struct ComponentSet
{
	using GetBaseEntity = BaseEntity<FirstComponent, OtherComponents...>;
	template<class Implementation>
	using GetEntity = Entity<Implementation, FirstComponent, OtherComponents...>;
};

class Flyable
{
public:
	Flyable ( ) = default;
	virtual ~Flyable ( ) = default;

	virtual void Fly ( ) = 0;
};

class Clickable
{
public:
	Clickable ( ) = default;
	virtual ~Clickable ( ) = default;

	virtual void OnClick ( ) = 0;
};

class Renderable
{
public:
	Renderable ( ) = default;
	Renderable ( const std::string& name ) : name_ { name }
	{

	}

	void Render ( )
	{
		std::cout << "Rendered " << name_ << '\n';
	}

private:
	std::string name_;
};

class Button : public Clickable, public Renderable
{
public:
	Button ( ) = default;
	Button ( const std::string& name ) : Renderable { name }
	{

	}

	void OnClick ( ) override
	{
		std::cout << "Clickety click click!" << '\n';
	}
};

class Bird : public Flyable, public Renderable
{
public:
	Bird ( ) = default;
	Bird ( const std::string& name ) : Renderable { name }
	{

	}

	void Fly ( ) override
	{
		std::cout << "Flappety flap flap!" << '\n';
	}
};

class Box
{
public:
	Box ( ) = default;
	Box ( const std::string& name ) : renderable_ { name }
	{

	}

	operator Renderable& ( )
	{
		return renderable_;
	}

private:
	Renderable renderable_;
};

int main ( )
{
	Entity<Box, Flyable, Clickable, Renderable> entity;
	entity.GetImplementation ( ) = Box { "lel" };

	Renderable* renderable = entity.GetComponent<Renderable> ( );

	if ( renderable != nullptr )
	{
		renderable->Render ( );
	}

	system ( "pause" );
	return 0;
}