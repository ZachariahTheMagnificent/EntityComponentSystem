#include <iostream>
#include <string>
#include "Entity.h"
#include <vector>
#include <memory>

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

	void Render ( ) const
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
	operator const Renderable& ( ) const
	{
		return renderable_;
	}

private:
	Renderable renderable_;
};

class Base
{
public:
	template<typename...Types>
	Base ( Types&&...arguments ) : x { std::forward<Types>(arguments)... }
	{

	}

public:
	int x;
};

class Derived : public Base
{
public:
	using Base::Base;
};

class NextDerived : public Derived
{
public:
	using Derived::Derived;
};

class NextNextDerived : public NextDerived
{
public:
	using NextDerived::NextDerived;
};

int main ( )
{
	Derived d { 1 };
	NextDerived e { 1 };
	NextNextDerived f { 1 };


	using ComponentSet = ComponentSet<Flyable, Clickable, Renderable>;

	using BaseEntity = ComponentSet::GetBaseEntity;

	using ButtonEntity = ComponentSet::GetEntity<Button>;
	using BirdEntity = ComponentSet::GetEntity<Bird>;
	using BoxEntity = ComponentSet::GetEntity<Box>;

	BoxEntity B { "button" };
	Flyable* r=B.GetComponent<Flyable> ( );
	
	//if ( r != nullptr )
	//{
	//	r->Render ( );
	//}
	system ( "pause" );
	return 0;
}