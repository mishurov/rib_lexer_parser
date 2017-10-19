/* ************************************************************************
 * Copyright 2017 Alexander Mishurov
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ************************************************************************/

%{
#include "parser/rib_lexer.h"
using token = rib::Parser::token;

#define yyterminate() return(token::END)
#undef  YY_DECL
#define YY_DECL int rib::Lexer::yylex( \
    rib::Parser::semantic_type * const lval, \
    rib::Parser::location_type *loc)
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);
#define YY_NO_UNISTD_H
%}

%option nodefault
%option noyywrap
%option yyclass="rib::Lexer"
%option c++

%x SHARP_COMMENT

%%

%{
    yylval = lval;
%}

Display { return(token::DISPLAY); }
Format { return(token::FORMAT); }
Projection { return(token::PROJECTION); }
Option { return(token::OPTION); }
version { return(token::VERSION); }
PixelSamples { return(token::PIXEL_SAMPLES); }
PixelFilter { return(token::PIXEL_FILTER); }
ShadingRate { return(token::SHADING_RATE); }
Translate { return(token::TRANSLATE); }
Rotate { return(token::ROTATE); }
Scale { return(token::SCALE); }
WorldBegin { return(token::WORLD_BEGIN); }
WorldEnd { return(token::WORLD_END); }
AttributeBegin { return(token::ATTRIBUTE_BEGIN); }
AttributeEnd { return(token::ATTRIBUTE_END); }
Attribute { return(token::ATTRIBUTE); }
TransformBegin { return(token::TRANSFORM_BEGIN); }
TransformEnd { return(token::TRANSFORM_END); }
LightSource { return(token::LIGHT_SOURCE); }
Surface { return(token::SURFACE); }
Color { return(token::COLOR); }
Opacity { return(token::OPACITY); }
Sides { return(token::SIDES); }
Orientation { return(token::ORIENTATION); }
Hyperboloid { return(token::HYPERBOLOID); }
Paraboloid { return(token::PARABOLOID); }
Torus { return(token::TORUS); }
Cylinder { return(token::CYLINDER); }
Sphere { return(token::SPHERE); }
Disk { return(token::DISK); }
Cone { return(token::CONE); }
ConcatTransform { return(token::CONCAT_TRANSFORM); }
PointsGeneralPolygons { return(token::POINTS_GENERAL_POLYGONS); }
Pattern { return(token::PATTERN); }
Bxdf { return(token::BXDF); }
Light { return(token::LIGHT); }


#                   { BEGIN(SHARP_COMMENT); }
<SHARP_COMMENT>\n   { BEGIN(INITIAL); return(token::COMMENT); }
<SHARP_COMMENT>.    { }

\[  { return(token::LEFT_SQUARE_BRACKET); }

\]  { return(token::RIGHT_SQUARE_BRACKET); }

[-+]?[0-9]+     {
                    yylval->build<int>(atoi(yytext));
                    return(token::INT);
                }

[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?*     {
                                                yylval->build<float>(
                                                    atof(yytext)
                                                );
                                                return(token::FLOAT);
                                            }

\"(\\.|[^\\"])*\"   {
                        std::string str(yytext);
                        str = str.substr(1, str.size() - 2);
                        yylval->build<std::string>(str);
                        return(token::STRING);
                    }

\n  { loc->lines(); }

[\t\r ]+    { ; }

.   { return(token::UNKNOWN); }

%%
