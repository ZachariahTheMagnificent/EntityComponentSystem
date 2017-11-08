#pragma once
#include <type_traits>

template<typename Base, typename Component>
class ComponentList : public Base
{
public:
	using Base::Base;
	virtual ~ComponentList ( ) = default;

protected:
	template<typename NextComponent>
	using Add = ComponentList<ComponentList, NextComponent>;

	using Base::Get;

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
	using BaseEntity_<typename ComponentList<Base, CurrentComponent>::Add<Component>, OtherComponents...>::BaseEntity_;
	virtual ~BaseEntity_ ( ) = default;
};

template<typename List>
class BaseEntity_<List> : public List
{
public:
	using List::List;
	virtual ~BaseEntity_ ( ) = default;

	template<typename Component>
	Component* GetComponent ( )
	{
		const Component*const component = nullptr;
		return this->Get ( component );
	}
	template<typename Component>
	const Component* GetComponent ( ) const
	{
		const Component*const component = nullptr;
		return this->Get ( component );
	}
};

template<typename FirstComponent, typename...OtherComponents>
using BaseEntity = BaseEntity_<ComponentList<void, FirstComponent>, OtherComponents...>;

template<typename Base, typename Component>
class Dispatcher : public Base
{
public:
	using Base::Base;
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
		auto& implementation = this->GetImplementation ( );
		using ImplementationType = std::decay_t<decltype ( implementation )>;

		return Dispatch<ImplementationType>::Execute ( implementation );
	}
	const Component* Get ( const Component*const ) const override
	{
		auto& implementation = this->GetImplementation ( );
		using ImplementationType = std::decay_t<decltype ( implementation )>;

		return Dispatch<ImplementationType>::Execute ( implementation );
	}
};

template<typename Base, typename...Components>
class Entity_;

template<typename Base, typename FirstComponent, typename...OtherComponents>
class Entity_<Base, FirstComponent, OtherComponents...> : public Entity_<Dispatcher<Base, FirstComponent>, OtherComponents...>
{
public:
	using Entity_<Dispatcher<Base, FirstComponent>, OtherComponents...>::Entity_;
	virtual ~Entity_ ( ) = default;
};

template<typename Base, typename CurrentComponent, typename Component, typename...OtherComponents>
class Entity_<Dispatcher<Base, CurrentComponent>, Component, OtherComponents...> : public Entity_<Dispatcher<Base, CurrentComponent>::Add<Component>, OtherComponents...>
{
public:
	using Entity_<Dispatcher<Base, CurrentComponent>::Add<Component>, OtherComponents...>::Entity_;
	virtual ~Entity_ ( ) = default;
};

template<typename Base>
class Entity_<Base> : public Base
{
public:
	using Base::Base;
	virtual ~Entity_ ( ) = default;
};

template<typename Implementation, typename FirstComponent, typename...OtherComponents>
class Implementor : public BaseEntity<FirstComponent, OtherComponents...>
{
public:
	using BaseEntity = BaseEntity<FirstComponent, OtherComponents...>;

	using BaseEntity::BaseEntity;
	template<typename...Types>
	Implementor ( Types&&...arguments ) : implementation_ { std::forward<Types> ( arguments )... }
	{

	}
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