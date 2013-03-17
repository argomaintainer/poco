//
// Object.h
//
// $Id$
//
// Library: JSON
// Package: JSON
// Module:  Object
//
// Definition of the Object class.
//
// Copyright (c) 2012, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef JSON_Object_INCLUDED
#define JSON_Object_INCLUDED


#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Array.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/SharedPtr.h"
#include "Poco/Dynamic/Var.h"
#include <map>
#include <vector>
#include <deque>
#include <iostream>
#include <sstream>


namespace Poco {
namespace JSON {


class JSON_API Object
	/// Represents a JSON object.
{
public:
	typedef SharedPtr<Object> Ptr;

	Object(bool preserveInsertionOrder = false);
		/// Default constructor. If preserveInsertionOrder, object
		/// will preserve the items insertion order. Otherwise, items
		/// will be sorted by keys.

	Object(const Object& copy);
		/// Copy constructor

	virtual ~Object();
		/// Destructor

	Dynamic::Var get(const std::string& key) const;
		/// Retrieves a property. An empty value is
		/// returned when the property doesn't exist.

	Array::Ptr getArray(const std::string& key) const;
		/// Returns a SharedPtr to an array when the property
		/// is an array. An empty SharedPtr is returned when
		/// the element doesn't exist or is not an array.

	Object::Ptr getObject(const std::string& key) const;
		/// Returns a SharedPtr to an object when the property
		/// is an object. An empty SharedPtr is returned when
		/// the property doesn't exist or is not an object

	template<typename T>
	T getValue(const std::string& key) const
		/// Retrieves the property with the given name and will
		/// try to convert the value to the given template type.
		/// The convert<T> method of Dynamic is called
		/// which can also throw exceptions for invalid values.
		/// Note: This will not work for an array or an object.
	{
		Dynamic::Var value = get(key);
		return value.convert<T>();
	}

	void getNames(std::vector<std::string>& names) const;
		/// Returns all property names

	bool has(const std::string& key) const;
		/// Returns true when the given property exists

	bool isArray(const std::string& key) const;
		/// Returns true when the given property contains an array

	bool isNull(const std::string& key) const;
		/// Returns true when the given property contains a null value

	bool isObject(const std::string& key) const;
		/// Returns true when the given property contains an object

	template<typename T>
	T optValue(const std::string& key, const T& def) const
		/// Returns the value of a property when the property exists
		/// and can be converted to the given type. Otherwise
		/// def will be returned.
	{
		T value = def;
		ValueMap::const_iterator it = _values.find(key);
		if (it != _values.end() && ! it->second.isEmpty() )
		{
			try
			{
				value = it->second.convert<T>();
			}
			catch(...)
			{
				// The default value will be returned
			}
		}
		return value;
	}

	std::size_t size() const;
		/// Returns the number of properties

	void set(const std::string& key, const Dynamic::Var& value);
		/// Sets a new value

	void stringify(std::ostream& out, unsigned int indent = 0, int step = -1) const;
		/// Prints the object to out. When indent is 0, the object
		/// will be printed on a single line without indentation.

	void remove(const std::string& key);
		/// Removes the property with the given key

private:

	template <typename C>
	void doStringify(const C& container, std::ostream& out, unsigned int indent, int step) const
	{
		out << '{';

		if (indent > 0) out << std::endl;
		
		typename C::const_iterator it = container.begin();
		typename C::const_iterator end = container.end();
		for (; it != end;)
		{
			for(int i = 0; i < indent; i++) out << ' ';

			out << '"' << getKey(it) << '"';
			out << ((indent > 0) ? " : " : ":");

			Stringifier::stringify(getValue(it), out, indent + step, step);

			if ( ++it != container.end() ) out << ',';

			if (step > 0) out << '\n';
		}

		if (indent >= step) indent -= step;

		for (int i = 0; i < indent; i++)
			out << ' ';

		out << '}';
	}

	typedef std::map<std::string, Dynamic::Var> ValueMap;
	typedef std::deque<Dynamic::Var*> KeyPtrList;

	const std::string& getKey(ValueMap::const_iterator& it) const;
	const Dynamic::Var& getValue(ValueMap::const_iterator& it) const;
	const std::string& getKey(KeyPtrList::const_iterator& it) const;
	const Dynamic::Var& getValue(KeyPtrList::const_iterator& it) const;

	ValueMap   _values;
	KeyPtrList _keys;
	bool       _preserveInsOrder;
};


inline bool Object::has(const std::string& key) const
{
	ValueMap::const_iterator it = _values.find(key);
	return it != _values.end();
}


inline bool Object::isArray(const std::string& key) const
{
	ValueMap::const_iterator it = _values.find(key);
	return it != _values.end() && it->second.type() == typeid(Array::Ptr);
}


inline bool Object::isNull(const std::string& key) const
{
	ValueMap::const_iterator it = _values.find(key);
	return it == _values.end() || it->second.isEmpty();
}


inline bool Object::isObject(const std::string& key) const
{
	ValueMap::const_iterator it = _values.find(key);
	return it != _values.end() && it->second.type() == typeid(Object::Ptr);
}


inline std::size_t Object::size() const
{
	return static_cast<std::size_t>(_values.size());
}


inline void Object::remove(const std::string& key)
{
	_values.erase(key);
}


inline const std::string& Object::getKey(ValueMap::const_iterator& it) const
{
	return it->first;
}


inline const Dynamic::Var& Object::getValue(ValueMap::const_iterator& it) const
{
	return it->second;
}


inline const Dynamic::Var& Object::getValue(KeyPtrList::const_iterator& it) const
{
	return **it;
}


}} // Namespace Poco::JSON


namespace Poco {
namespace Dynamic {


template <>
class VarHolderImpl<JSON::Object::Ptr>: public VarHolder
{
public:
	VarHolderImpl(const JSON::Object::Ptr& val): _val(val)
	{
	}

	~VarHolderImpl()
	{
	}

	const std::type_info& type() const
	{
		return typeid(JSON::Object::Ptr);
	}

	void convert(Int8&) const
	{
		throw BadCastException();
	}

	void convert(Int16&) const
	{
		throw BadCastException();
	}

	void convert(Int32&) const
	{
		throw BadCastException();
	}

	void convert(Int64&) const
	{
		throw BadCastException();
	}

	void convert(UInt8&) const
	{
		throw BadCastException();
	}

	void convert(UInt16&) const
	{
		throw BadCastException();
	}

	void convert(UInt32&) const
	{
		throw BadCastException();
	}

	void convert(UInt64&) const
	{
		throw BadCastException();
	}

	void convert(bool& value) const
	{
		value = !_val.isNull() && _val->size() > 0;
	}

	void convert(float&) const
	{
		throw BadCastException();
	}

	void convert(double&) const
	{
		throw BadCastException();
	}

	void convert(char&) const
	{
		throw BadCastException();
	}

	void convert(std::string& s) const
	{
		std::ostringstream oss;
		_val->stringify(oss, 2);
		s = oss.str();
	}

	void convert(DateTime& /*val*/) const
	{
		//TODO: val = _val;
		throw NotImplementedException("Conversion not implemented: JSON:Object => DateTime");
	}

	void convert(LocalDateTime& /*ldt*/) const
	{
		//TODO: ldt = _val.timestamp();
		throw NotImplementedException("Conversion not implemented: JSON:Object => LocalDateTime");
	}

	void convert(Timestamp& /*ts*/) const
	{
		//TODO: ts = _val.timestamp();
		throw NotImplementedException("Conversion not implemented: JSON:Object => Timestamp");
	}

	VarHolder* clone(Placeholder<VarHolder>* pVarHolder = 0) const
	{
		return cloneHolder(pVarHolder, _val);
	}

	const JSON::Object::Ptr& value() const
	{
		return _val;
	}

	bool isArray() const
	{
		return false;
	}

	bool isInteger() const
	{
		return false;
	}

	bool isSigned() const
	{
		return false;
	}

	bool isNumeric() const
	{
		return false;
	}

	bool isString() const
	{
		return false;
	}

private:
	JSON::Object::Ptr _val;
};


}} // namespace Poco::JSON


#endif // JSON_Object_INCLUDED
