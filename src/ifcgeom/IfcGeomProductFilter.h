/********************************************************************************
 *                                                                              *
 * This file is part of IfcOpenShell.                                           *
 *                                                                              *
 * IfcOpenShell is free software: you can redistribute it and/or modify         *
 * it under the terms of the Lesser GNU General Public License as published by  *
 * the Free Software Foundation, either version 3.0 of the License, or          *
 * (at your option) any later version.                                          *
 *                                                                              *
 * IfcOpenShell is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 * Lesser GNU General Public License for more details.                          *
 *                                                                              *
 * You should have received a copy of the Lesser GNU General Public License     *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.         *
 *                                                                              *
 ********************************************************************************/

#ifndef IFCPRODUCTFILTER_H
#define IFCPRODUCTFILTER_H

#include "../ifcparse/IfcFile.h"

#include "../ifcgeom/IfcGeom.h"

namespace IfcGeom {
	
	class ProductFilter {
	protected:

		std::set<IfcSchema::Type::Enum> entities_to_include_or_exclude;
		bool include_entities_in_processing;

		void populate_set(const std::set<std::string>& include_or_ignore) {
			entities_to_include_or_exclude.clear();
			for (std::set<std::string>::const_iterator it = include_or_ignore.begin(); it != include_or_ignore.end(); ++it) {
				std::string uppercase_type = *it;
				for (std::string::iterator c = uppercase_type.begin(); c != uppercase_type.end(); ++c) {
					*c = toupper(*c);
				}
				IfcSchema::Type::Enum ty;
				try {
					ty = IfcSchema::Type::FromString(uppercase_type);
				} catch (const IfcParse::IfcException&) {
					std::stringstream ss;
					ss << "'" << *it << "' does not name a valid IFC entity";
					throw IfcParse::IfcException(ss.str());
				}
				entities_to_include_or_exclude.insert(ty);
				// TODO: Add child classes so that containment in set can be in O(log n)
			}
		}

	public:
		void includeEntities(const std::set<std::string>& entities) {
			populate_set(entities);
			include_entities_in_processing = true;
		}

		void excludeEntities(const std::set<std::string>& entities) {
			populate_set(entities);
			include_entities_in_processing = false;
		}

	protected:
		void _initialize() {
			// Upon initialisation, the (empty) set of entity names,
			// should be excluded, or no products would be processed.
			include_entities_in_processing = false;
		}

	public:
		ProductFilter() {}

		~ProductFilter() {}

		virtual bool shouldConvertProduct(IfcSchema::IfcProduct* product)
		{
			// Filter the products based on the set of entities being included or excluded for
			// processing. The set is iterated over te able to filter on subtypes.
			bool found = false;
			for (std::set<IfcSchema::Type::Enum>::const_iterator jt = entities_to_include_or_exclude.begin(); jt != entities_to_include_or_exclude.end(); ++jt) {
				if (product->is(*jt)) {
					found = true;
					break;
				}
			}

			if (found == include_entities_in_processing) {
				return true;
			}

			return false;
		}
	};
}

#endif
