/*
 * Copyright 2016 <copyright holder> <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "innermodel/innermodeldisplay.h"

InnerModelDisplay::InnerModelDisplay(QString id_, uint32_t port_, QString texture_, float width_, float height_,float depth_, int repeat_, float nx_, float ny_, float nz_, float px_, float py_, float pz_, bool collidable_, InnerModelNode *parent_) : InnerModelNode(id_, parent_)
{
#if FCL_SUPPORT==1
	collisionObject = NULL;
#endif
	if ( abs(nx_)<0.001 and abs(ny_)<0.001 and abs(nz_)<0.001 ) nz_ = -1;
	normal = QVec::vec3(nx_, ny_, nz_);
	point = QVec::vec3(px_, py_, pz_);
	nx = ny = nz = px = py = pz = NULL;
	texture = texture_;
	width = width_;
	height = height_;
	depth = depth_;
	repeat = repeat_;
	collidable = collidable_;
	port = port_;

	id = id_;

#if FCL_SUPPORT==1
	std::vector<fcl::Vec3f> vertices;
	vertices.push_back(fcl::Vec3f(-width/2., +height/2., -depth/2.)); // Front NW
	vertices.push_back(fcl::Vec3f(+width/2., +height/2., -depth/2.)); // Front NE
	vertices.push_back(fcl::Vec3f(-width/2., -height/2., -depth/2.)); // Front SW
	vertices.push_back(fcl::Vec3f(+width/2., -height/2., -depth/2.)); // Front SE
	vertices.push_back(fcl::Vec3f(-width/2., +height/2., +depth/2.)); // Back NW
	vertices.push_back(fcl::Vec3f(+width/2., +height/2., +depth/2.)); // Back NE
	vertices.push_back(fcl::Vec3f(-width/2., -height/2., +depth/2.)); // Back SW
	vertices.push_back(fcl::Vec3f(+width/2., -height/2., +depth/2.)); // Back SE

	osg::Matrix r;
	r.makeRotate(osg::Vec3(0, 0, 1), osg::Vec3(normal(0), normal(1), -normal(2)));
	QMat qmatmat(4,4);
	for (int rro=0; rro<4; rro++)
	{
		for (int cco=0; cco<4; cco++)
		{
			qmatmat(rro,cco) = r(rro,cco);
		}
	}

	for (size_t i=0; i<vertices.size(); i++)
	{
		fcl::Vec3f v = vertices[i];
		const QVec rotated = (qmatmat*(QVec::vec3(v[0], v[1], v[2]).toHomogeneousCoordinates())).fromHomogeneousCoordinates();
		vertices[i] = fcl::Vec3f(rotated(0)+px_, rotated(1)+py_, rotated(2)+pz_);
	}

	std::vector<fcl::Triangle> triangles;
	triangles.push_back(fcl::Triangle(0,1,2)); // Front
	triangles.push_back(fcl::Triangle(1,2,3));
	triangles.push_back(fcl::Triangle(4,5,6)); // Back
	triangles.push_back(fcl::Triangle(5,6,7));
	triangles.push_back(fcl::Triangle(4,0,6)); // Left
	triangles.push_back(fcl::Triangle(0,6,2));
	triangles.push_back(fcl::Triangle(5,1,7)); // Right
	triangles.push_back(fcl::Triangle(1,7,3));
	triangles.push_back(fcl::Triangle(5,1,4)); // Top
	triangles.push_back(fcl::Triangle(1,4,0));
	triangles.push_back(fcl::Triangle(2,3,6)); // Bottom
	triangles.push_back(fcl::Triangle(3,6,7));

	fclMesh = FCLModelPtr(new FCLModel());
	fclMesh->beginModel();
	fclMesh->addSubModel(vertices, triangles);
	fclMesh->endModel();
	collisionObject = new fcl::CollisionObject(fclMesh);

#endif
}

void InnerModelDisplay::updateTexture(QString texture_)
{
  texture = texture_;
}

void InnerModelDisplay::print(bool verbose)
{
	if (verbose) normal.print(QString("Display: ")+id);
}

void InnerModelDisplay::update()
{
	if (fixed)
	{
	}
	updateChildren();
}

void InnerModelDisplay::save(QTextStream &out, int tabs)
{
	for (int i=0; i<tabs; i++) out << "\t";
	out << "<display id=\"" << id << "\" texture=\"" << texture << "\" size=\"" << QString::number(width,'g', 10)<<","<<QString::number( height,'g', 10)<<","
	<<QString::number( depth,'g', 10) << "\" repeat=\"" << QString::number(repeat, 'g', 10) << "\" nx=\"" << QString::number(normal(0), 'g', 10)
	<< "\" ny=\"" << QString::number(normal(1), 'g', 10) << "\" nz=\""
	<< QString::number(normal(2), 'g', 10) << "\" px=\"" << QString::number(point(0), 'g', 10) << "\" py=\"" << QString::number(point(1), 'g', 10)
	<< "\" pz=\"" << QString::number(point(2), 'g', 10) <<"\" collide=\""<< QString::number(collidable,'g',10) << "\"" << " port=\"" << port <<"\" />\n";
}

void InnerModelDisplay::setUpdatePointers(float *nx_, float *ny_, float *nz_, float *px_, float *py_, float *pz_)
{
	nx = nx_;
	ny = ny_;
	nz = nz_;
	px = px_;
	py = py_;
	pz = pz_;
	nx = ny = nz = px = py = pz = NULL;
	fixed = false;
}

void InnerModelDisplay::update(float nx_, float ny_, float nz_, float px_, float py_, float pz_)
{
	normal(0) = nx_;
	normal(1) = ny_;
	normal(2) = nz_;
	point(0) = px_;
	point(1) = py_;
	point(2) = pz_;
	fixed = true;
}

InnerModelNode * InnerModelDisplay::copyNode(QHash<QString, InnerModelNode *> &hash, InnerModelNode *parent)
{
	InnerModelDisplay *ret = new InnerModelDisplay(id, port, texture, width, height, depth, repeat, normal(0), normal(1), normal(2), point(0), point(1), point(2), parent);
	ret->level = level;
	ret->fixed = fixed;
	ret->children.clear();
	ret->attributes.clear();
	hash[id] = ret;

	ret->innerModel = parent->innerModel;
	for (QList<InnerModelNode*>::iterator i=children.begin(); i!=children.end(); i++)
	{
		ret->addChild((*i)->copyNode(hash, ret));
	}

	return ret;
}
