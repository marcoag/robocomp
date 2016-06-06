/*
[[[cog

import sys
sys.path.append('/opt/robocomp/python')

import cog
def A():
	cog.out('<@@<')
def Z():
	cog.out('>@@>')
def TAB():
	cog.out('<TABHERE>')
from parseSMDSL import *
from parseCDSL import *
from parseIDSL import *
component = CDSLParsing.fromFile(theCDSL)
sm = SMDSLparsing.fromFile(component['statemachine'])
if component == None:
	print('Can\'t locate', theCDSLs)
	sys.exit(1)

pool = IDSLPool(theIDSLs)


]]]
[[[end]]]
 *    Copyright (C) 
[[[cog
A()
import datetime
cog.out(str(datetime.date.today().year))
Z()
]]]
[[[end]]]
 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "genericworker.h"
/**
* \brief Default constructor
*/
GenericWorker::GenericWorker(MapPrx& mprx) :
[[[cog
if component['gui'] != 'none':
	cog.outl("""#ifdef USE_QTGUI
Ui_guiDlg()
#else
QObject()
#endif
""")
else:
	cog.outl("QObject()")
]]]
[[[end]]]
{

[[[cog
if component['statemachine'] != 'none':
    if sm['machine']['contents']['transition'] != "none":
        for transi in sm['machine']['contents']['transition']:
            cod = "<TABHERE>" + transi['src'] + "->addTransition("
            for dest in transi['dest']:
                aux=cod
                aux+="this, SIGNAL("+transi['src'] + "to" + dest+"()), " + dest + ");"
                cog.outl(aux)
    if sm['substates'] != "none":
        for substates in sm['substates']:
            if substates['contents']['transition'] != "none":
                for transi in substates['contents']['transition']:
                    cod = "<TABHERE>" + transi['src'] + "->addTransition("
                    for dest in transi['dest']:
                        aux=cod
                        aux+="this, SIGNAL("+transi['src'] + "to" + dest+"()), " + dest + ");"
                        cog.outl(aux)
    for state in sm['machine']['contents']['states']:
        cod = "<TABHERE>" + sm['machine']['name'] +  ".addState(" + state + ");"
        cog.outl(cod)
    for state in sm['machine']['contents']['states']:
        cod = "<TABHERE>QObject::connect(" + state + ", SIGNAL(entered()), this, SLOT(fun_" + state + "()));"
        cog.outl(cod)
    if sm['substates'] != "none":
        for substates in sm['substates']:
            for state in substates['contents']['states']:
                cod = "<TABHERE>QObject::connect(" + state + ", SIGNAL(entered()), this, SLOT(fun_" + state + "()));"
                cog.outl(cod)
]]]
[[[end]]]
[[[cog
for name, num in getNameNumber(component['requires']):
	cog.outl("<TABHERE>"+name.lower()+num+"_proxy = (*("+name+"Prx*)mprx[\""+name+"Proxy"+num+"\"]);")	
]]]
[[[end]]]

[[[cog
for name, num in getNameNumber(component['publishes']):
	cog.outl("<TABHERE>"+name.lower()+num+"_proxy = (*("+name+"Prx*)mprx[\""+name+"Pub"+num+"\"]);")
]]]
[[[end]]]

	mutex = new QMutex(QMutex::Recursive);

[[[cog
if component['gui'] != 'none':
	cog.outl("""<TABHERE>#ifdef USE_QTGUI
		setupUi(this);
		show();
	#endif""")
]]]
[[[end]]]
	Period = BASIC_PERIOD;
	connect(&timer, SIGNAL(timeout()), this, SLOT(compute()));
// 	timer.start(Period);
}

/**
* \brief Default destructor
*/
GenericWorker::~GenericWorker()
{

}
void GenericWorker::killYourSelf()
{
	rDebug("Killing myself");
	emit kill();
}
/**
* \brief Change compute period
* @param per Period in ms
*/
void GenericWorker::setPeriod(int p)
{
	rDebug("Period changed"+QString::number(p));
	Period = p;
	timer.start(Period);
}

[[[cog
try:
	if 'agmagent' in [ x.lower() for x in component['options'] ]:
		cog.outl("""RoboCompPlanning::Action GenericWorker::createAction(std::string s)  // ESTO PODRIA ESTAR AUTOGENERADO
	{
		// Remove useless characters
		char chars[]="()";
	    for (unsigned int i=0; i<strlen(chars); ++i)
		{
			s.erase(std::remove(s.begin(), s.end(), chars[i]), s.end());
		}
	
	    // Initialize string parsing
		RoboCompPlanning::Action ret;
		istringstream iss(s);
	
		// Get action (first segment)
		if (not iss)
		{
			printf("agent %s: received invalid action (%s) -> (%d)\\n", PROGRAM_NAME, __FILE__, __LINE__);
			exit(-1);
		}
		else
		{
			iss >> ret.name;
		}
	
		do
		{
			std::string ss;
			iss >> ss;
			ret.symbols.push_back(ss);
		} while (iss);
	
		return ret;
	}	
	
	RoboCompAGMWorldModel::BehaviorResultType GenericWorker::status()
	{
		if (active)
			return RoboCompAGMWorldModel::StatusActive;
		return RoboCompAGMWorldModel::StatusIdle;
	}
	
	bool GenericWorker::activate(const BehaviorParameters &prs)
	{
		printf("Worker::activate\\n");
		mutex->lock();
		p = prs;
		active = true;
		iter = 0;
		mutex->unlock();
		return active;
	}
	
	bool GenericWorker::deactivate() 
	{
		printf("Worker::deactivate\\n");
		mutex->lock();
		active = false;
		iter = 0;
		mutex->unlock();
		return active;
	}
	
	bool GenericWorker::setParametersAndPossibleActivation(const ParameterMap &prs, bool &reactivated)
	{
		// We didn't reactivate the component
		reactivated = false;
	
		// Update parameters
		for (ParameterMap::const_iterator it=prs.begin(); it!=prs.end(); it++)
		{
			params[it->first] = it->second;
		}
	
		try
		{
			// Action
			p.action = createAction(params["action"].value);
	
			// Fill received plan
			p.plan.clear();
			QStringList actionList = QString::fromStdString(params["plan"].value).split(QRegExp("[()]+"), QString::SkipEmptyParts);
			for (int32_t actionString=0; actionString<actionList.size(); actionString++)
			{
				std::vector<string> elementsVec;
				QStringList elements = actionList[actionString].remove(QChar('\\n')).split(QRegExp("\\\\s+"), QString::SkipEmptyParts);
				for (int32_t elem=0; elem<elements.size(); elem++)
				{
					elementsVec.push_back(elements[elem].toStdString());
				}
				p.plan.push_back(elementsVec);
			}
		}
		catch (...)
		{
			return false;
		}
	
		// Check if we should reactivate the component
		if (isActive())
		{
			activate(p);
			reactivated = true;
		}
	
		return true;
	}""")
except:
	pass

]]]
[[[end]]]
