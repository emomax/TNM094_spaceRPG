#pragma once

#include "GameObject.h"
#include "Includes.h"

//#include <osgDB/ReadFile>
//#include <osg/MatrixTransform>
//#include <osg/ComputeBoundsVisitor>
//
//#include <osg/Depth>
//#include <osg/TexGen>
//#include <osg/TextureCubeMap>
//#include <osg/ShapeDrawable>
//#include <osg/Geode>
//#include <osgDB/ReadFile>
//#include <osgUtil/CullVisitor>

class Projectile : public GameObject
{
public:
	Projectile()
	{
		damage = 0.0;
		initTransform();
	}

	Projectile(std::string _name, osg::Vec3f _pos, osg::Vec3f _dir, osg::Quat _orientation, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, float _dmg, float _vel, bool _playerOwned);

	int getDmg() { return damage; }
	float getLifeTime() { return lifeTime; }
	void setLifeTime(float _t) { lifeTime = _t; }
	bool getPlayerOwned() { return playerOwned; }

	Projectile operator=(Projectile _p);

	virtual ~Projectile() {}

private:
	int damage;
	float lifeTime;
	bool playerOwned = false;
};
