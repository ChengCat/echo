#include "object.h"
#include <thirdparty/pugixml/pugixml.hpp>
#include <thirdparty/pugixml/pugiconfig.hpp>

namespace Echo
{
	Object::Object()
	{
	}

	// get class name
	const String& Object::getClassName() const
	{
		static String className = "Object";
		return className;
	}

	// propertys (script property or dynamic property)
	const PropertyInfos& Object::getPropertys() const 
	{ 
		return m_propertys; 
	}

	// register property
	bool Object::registerProperty(const String& className, const String& propertyName, const Variant::Type type)
	{
		PropertyInfoDynamic* info = EchoNew(PropertyInfoDynamic);
		info->m_name = propertyName;
		info->m_type = type;
		info->m_className = className;

		m_propertys.push_back(info);

		return true;
	}

	// instance res
	Object* Object::instanceObject(void* pugiNode)
	{
		pugi::xml_node* xmlNode = (pugi::xml_node*)pugiNode;

		Echo::String name = xmlNode->attribute("name").value();
		Echo::String className = xmlNode->attribute("class").value();

		Object* res = Echo::Class::create<Object*>(className);
		if (res)
		{
			res->setName(name);

			loadPropertyRecursive(pugiNode, res, className);

			return res;
		}

		return  nullptr;
	}

	// remember property recursive
	void Object::loadPropertyRecursive(void* pugiNode, Echo::Object* classPtr, const Echo::String& className)
	{
		pugi::xml_node* xmlNode = (pugi::xml_node*)pugiNode;

		// load parent property first
		Echo::String parentClassName;
		if (Echo::Class::getParentClass(parentClassName, className))
		{
			// don't display property of object
			if (parentClassName != "Object")
				loadPropertyRecursive(pugiNode, classPtr, parentClassName);
		}

		const Echo::PropertyInfos& propertys = Echo::Class::getPropertys(className);
		for (const Echo::PropertyInfo* prop : propertys)
		{
			Echo::Variant var;
			String valueStr = xmlNode->attribute(prop->m_name.c_str()).value();
			var.fromString(prop->m_type, valueStr);

			Class::setPropertyValue(classPtr, prop->m_name, var);
		}
	}
}