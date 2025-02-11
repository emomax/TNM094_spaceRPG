#include "Object.h"

void Object::initTransform()
{
	transform = new osg::MatrixTransform();
	transform->setMatrix(osg::Matrix::identity());
}

void Object::translate(osg::Vec3f _t)
{
	pos = pos + _t;
	transform->setMatrix(osg::Matrix::identity());
	transform->postMult(osg::Matrix::rotate(orientation));
	transform->postMult(osg::Matrix::translate(pos));
}

void Object::rotate(osg::Quat _q)
{
	orientation = orientation * _q;
	transform->setMatrix(osg::Matrix::identity());
	transform->postMult(osg::Matrix::rotate(orientation));
	transform->postMult(osg::Matrix::translate(pos));
}
