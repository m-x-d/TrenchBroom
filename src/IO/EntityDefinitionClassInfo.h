/*
 Copyright (C) 2010-2013 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TrenchBroom__EntityDefinitionClassInfo__
#define __TrenchBroom__EntityDefinitionClassInfo__

#include "TrenchBroom.h"
#include "VecMath.h"
#include "StringUtils.h"
#include "Color.h"
#include "Assets/AssetTypes.h"

#include <map>

namespace TrenchBroom {
    namespace IO {
        class EntityDefinitionClassInfo;
        typedef std::map<String, EntityDefinitionClassInfo> EntityDefinitionClassInfoMap;
        
        class EntityDefinitionClassInfo {
        private:
            size_t m_line;
            size_t m_column;
            String m_name;
            String m_description;
            bool m_hasDescription;
            Color m_color;
            bool m_hasColor;
            BBox3 m_size;
            bool m_hasSize;
            Assets::PropertyDefinitionMap m_properties;
            Assets::ModelDefinitionList m_models;
        public:
            EntityDefinitionClassInfo();
            EntityDefinitionClassInfo(const size_t line, const size_t column, const Color& defaultColor);
            
            size_t line() const;
            size_t column() const;
            const String& name() const;
            const String& description() const;
            bool hasDescription() const;
            const Color& color() const;
            bool hasColor() const;
            const BBox3& size() const;
            bool hasSize() const;
            Assets::PropertyDefinitionList propertyList() const;
            const Assets::PropertyDefinitionMap& propertyMap() const;
            const Assets::ModelDefinitionList& models() const;

            void setName(const String& name);
            void setDescription(const String& description);
            void setColor(const Color& color);
            void setSize(const BBox3& size);
            void addPropertyDefinition(Assets::PropertyDefinitionPtr propertyDefinition);
            void addPropertyDefinitions(const Assets::PropertyDefinitionList& propertyDefinitions);
            void addPropertyDefinitions(const Assets::PropertyDefinitionMap& propertyDefinitions);
            void addModelDefinition(Assets::ModelDefinitionPtr modelDefinition);
            void addModelDefinitions(const Assets::ModelDefinitionList& modelDefinitions);
        
            void resolveBaseClasses(const EntityDefinitionClassInfoMap& baseClasses, const StringList& classnames);
        private:
            static void mergeProperties(Assets::PropertyDefinition* classProperty, const Assets::PropertyDefinition* baseclassProperty);
        };
    }
}

#endif /* defined(__TrenchBroom__EntityDefinitionClassInfo__) */
