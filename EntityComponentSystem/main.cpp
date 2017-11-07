#include <iostream>
#include <type_traits>
#include <string>

template<typename Base, typename Component>
class ComponentList : public Base
{
protected:
	template<typename Component, typename TestEntity>
	struct GetElement_;

	template<typename Component, typename Base, typename OtherComponent>
	struct GetElement_<Component, ComponentList<Base, OtherComponent>>
	{
		using Type = typename GetElement_<Component, Base>::Type;
	};

	template<typename Component, typename Base>
	struct GetElement_<Component, ComponentList<Base, Component>>
	{
		using Type = ComponentList<Base, Component>;
	};

	template<typename Component>
	struct GetElement_<Component, ComponentList<void, Component>>
	{
		using Type = ComponentList<void, Component>;
	};

	template<typename NextComponent>
	using Add = ComponentList<ComponentList, NextComponent>;

	template<class Component>
	using GetElement = typename GetElement_<Component, ComponentList>::Type;

public:
	ComponentList ( ) = default;
	virtual ~ComponentList ( ) = default;

	virtual Component* Get ( const Component*const ) = 0;
};

template<typename Component>
class ComponentList<void, Component>
{
protected:
	template<typename NextComponent>
	using Add = ComponentList<ComponentList, NextComponent>;

	template<class Component>
	using GetElement = ComponentList;

public:
	ComponentList ( ) = default;
	virtual ~ComponentList ( ) = default;

	virtual Component* Get ( const Component*const ) = 0;
};

template<class List, class...Components>
class BaseEntity_;

template<class Base, class CurrentComponent, class Component, class...OtherComponents>
class BaseEntity_<ComponentList<Base, CurrentComponent>, Component, OtherComponents...> : public BaseEntity_<typename ComponentList<Base, CurrentComponent>::Add<Component>, OtherComponents...>
{
public:
	BaseEntity_ ( ) = default;
	virtual ~BaseEntity_ ( ) = default;
};

template<class List>
class BaseEntity_<List> : private List
{
public:
	BaseEntity_ ( ) = default;
	virtual ~BaseEntity_ ( ) = default;

	template<class Component>
	Component* GetComponent ( )
	{
		using Element = typename List::GetElement<Component>;

		const Component*const ptr = nullptr;
		return static_cast<Element*>( this )->Get ( ptr );
	}
};

template<class FirstComponent, class...OtherComponents>
using BaseEntity = BaseEntity_<ComponentList<void, FirstComponent>, OtherComponents...>;

template<class Base, class Component>
class Dispatcher : public Base
{
public:
	template<class Component>
	using Add = Dispatcher<Dispatcher, Component>;

	template<class Implementation, bool Convertible = std::is_convertible<Implementation, Component&>::value>
	struct Dispatch
	{
		static Component* Do ( Implementation& implementation )
		{
			return &static_cast< Component& >( implementation );
		}
	};
	template<class Implementation>
	struct Dispatch<Implementation, false>
	{
		static Component* Do ( Implementation& )
		{
			return nullptr;
		}
	};

	Component* Get ( const Component*const ) override
	{
		auto& implementation = GetImplementation ( );
		using ImplementationType = decltype ( implementation );

		return Dispatch<ImplementationType>::Do ( implementation );
	}
};

template<class Base, class...Components>
class Entity_;

template<class Base, class FirstComponent, class...OtherComponents>
class Entity_<Base, FirstComponent, OtherComponents...> : public Entity_<Dispatcher<Base, FirstComponent>, OtherComponents...>
{

};

template<class Base, typename CurrentComponent, class Component, class...OtherComponents>
class Entity_<Dispatcher<Base, CurrentComponent>, Component, OtherComponents...> : public Entity_<Dispatcher<Base, CurrentComponent>::Add<Component>, OtherComponents...>
{

};

template<class Base>
class Entity_<Base> : public Base
{

};

template<class Implementation, class FirstComponent, class...OtherComponents>
class Implementor : public BaseEntity<FirstComponent, OtherComponents...>
{
public:
	using Base = BaseEntity<FirstComponent, OtherComponents...>;

	Implementation& GetImplementation ( )
	{
		return implementation_;
	}

private:
	Implementation implementation_;
};

template<typename Implementation, typename...Components>
using Entity = Entity_<Implementor<Implementation, Components...>, Components...>;

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