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

int main ( )
{
	using ComponentSet = ComponentSet<Flyable, Clickable, Renderable>;

	using BaseEntity = ComponentSet::GetBaseEntity;

	using ButtonEntity = ComponentSet::GetEntity<Button>;
	using BirdEntity = ComponentSet::GetEntity<Bird>;
	using BoxEntity = ComponentSet::GetEntity<Box>;

	auto HandleEntity = [ ] ( BaseEntity& entity )
	{
		const auto clickable = entity.GetComponent<Clickable> ( );
		const auto flyable = entity.GetComponent<Flyable> ( );
		const auto renderable = entity.GetComponent<Renderable> ( );

		if ( clickable )
		{
			clickable->OnClick ( );
		}
		if ( flyable )
		{
			flyable->Fly ( );
		}
		if ( renderable )
		{
			renderable->Render ( );
		}
	};

	std::vector<std::unique_ptr<BaseEntity>> entities_;
	entities_.reserve ( 3 );
	entities_.push_back ( std::make_unique<ButtonEntity> ( "butt" ) );
	entities_.push_back ( std::make_unique<BirdEntity> ( "birb" ) );
	entities_.push_back ( std::make_unique<BoxEntity> ( "snake" ) );

	for ( const auto& entity : entities_ )
	{
		HandleEntity ( *entity );
	}

	system ( "pause" );
	return 0;
}