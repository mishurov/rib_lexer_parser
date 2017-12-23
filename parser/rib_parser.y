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

%skeleton "lalr1.cc"
%require  "3.0"

%defines
%define api.namespace { rib }
%define parser_class_name { Parser }

%code requires {
    namespace rib {
        class Lexer;
        class Driver;
    }
}

%parse-param { Lexer &lexer }
%parse-param { Driver &driver }

%code {
    #include "parser/rib_driver.h"

    #undef yylex
    #define yylex lexer.yylex
}

/*
https://www.gnu.org/software/bison/manual/html_node/_0025define-Summary.html
*/
%define api.value.type variant

%token END 0 "end of file"
%token UNKNOWN
%token <std::string> STRING
%token <int> INT
%token <float> FLOAT
%token LEFT_SQUARE_BRACKET
%token RIGHT_SQUARE_BRACKET

%token VERSION
%token DISPLAY
%token FORMAT
%token PROJECTION
%token OPTION
%token PIXEL_SAMPLES
%token PIXEL_FILTER
%token SHADING_RATE
%token HIDER
%token INTEGRATOR
%token PIXEL_VARIANCE
%token EXPOSURE
%token SCOPED_COORDINATE_SYSTEM

%token TRANSLATE
%token ROTATE
%token SCALE
%token CONCAT_TRANSFORM

%token WORLD_BEGIN
%token WORLD_END
%token ATTRIBUTE_BEGIN
%token ATTRIBUTE_END
%token ATTRIBUTE
%token TRANSFORM_BEGIN
%token TRANSFORM_END

%token LIGHT_SOURCE
%token AREA_LIGHT_SOURCE
%token SURFACE
%token GEOMETRY
%token COLOR
%token OPACITY

%token SIDES
%token ORIENTATION

%token HYPERBOLOID
%token PARABOLOID
%token TORUS
%token CYLINDER
%token SPHERE
%token DISK
%token CONE

%token POINTS_GENERAL_POLYGONS
%token POINTS_POLYGONS
%token PATTERN
%token BXDF
%token LIGHT

%type <float> float
%type <std::vector<float>*> float_list float_array
%type <std::vector<int>*> int_list int_array
%type <std::vector<std::string>*> string_list string_array

%locations

%%

rib : END | rib_item | rib rib_item;

rib_item
    : world_begin
    | world_end
    | attribute_begin
    | attribute_end
    | transform_begin
    | transform_end
    | sides
    | orientation
    | display
    | format
    | projection
    | option
    | integrator
    | hider
    | version
    | exposure
    | scoped_coordinate_system
    | pixel_samples
    | pixel_filter
    | pixel_variance
    | shading_rate
    | light_source
    | area_light_source
    | surface
    | geometry
    | color
    | opacity
    | translate
    | rotate
    | scale
    | hyperboloid
    | paraboloid
    | torus
    | cylinder
    | sphere
    | disk
    | cone
    | attribute
    | contcat_transform
    | points_general_polygons
    | points_polygons
    | pattern
    | bxdf
    | light
    ;

hyperboloid
    : HYPERBOLOID float float float float float float float
                { driver.addHyperboloid($2, $3, $4, $5, $6, $7, $8); }
    ;

paraboloid
    : PARABOLOID float float float float
                { driver.addParaboloid($2, $3, $4, $5); }
    ;

torus
    : TORUS float float float float float
                { driver.addTorus($2, $3, $4, $5, $6); }
    ;

cylinder
    : CYLINDER float float float float { driver.addCylinder($2, $3, $4, $5); }
    ;

sphere
    : SPHERE float float float float { driver.addSphere($2, $3, $4, $5); }
    ;

disk : DISK float float float { driver.addDisk($2, $3, $4); } ;

cone : CONE float float float { driver.addCone($2, $3, $4); } ;


points_general_polygons
    : points_general_polygons STRING string_array { delete $3; }
    | points_general_polygons STRING float_array
        {
            driver.addPGPparam($2, *$3);
            delete $3;
        }
    | POINTS_GENERAL_POLYGONS int_array int_array int_array
        {
            driver.addPGP(*$2, *$3, *$4);
            delete $2;
            delete $3;
            delete $4;
        }
    ;

points_polygons
    : points_polygons STRING float_array
        {
            driver.addPPparam($2, *$3);
            delete $3;
        }
    | POINTS_POLYGONS int_array int_array
        {
            driver.addPP(*$2, *$3);
            delete $2;
            delete $3;
        }
    ;

pattern
    : pattern STRING float_array
        {
            driver.addPatternFlParam($2, *$3);
            delete $3;
        }
    | pattern STRING string_array
        {
            driver.addPatternStrParam($2, *$3);
            delete $3;
        }
    | PATTERN STRING STRING
        {
            driver.addPattern($2, $3);
        }
    ;

bxdf
    : bxdf STRING float_array
        {
            driver.addBxdfFlParam($2, *$3);
            delete $3;
        }
    | bxdf STRING string_array
        {
            driver.addBxdfStrParam($2, *$3);
            delete $3;
        }
    | BXDF STRING STRING
        {
            driver.addBxdf($2, $3);
        }
    ;

light
    : light STRING float_array
        {
            driver.addLightFlParam($2, *$3);
            delete $3;
        }
    | light STRING string_array
        {
            driver.addLightStrParam($2, *$3);
            delete $3;
        }
    | LIGHT STRING STRING
        {
            driver.addLight($2, $3);
        }
    ;

attribute
    : attribute STRING float_array
        {
            driver.addAttrFlParam($2, *$3);
            delete $3; 
        }
    | attribute STRING string_array { delete $3; }
    | attribute STRING STRING
    | ATTRIBUTE STRING
        {
            driver.addAttribute($2);
        }
    ;


world_begin : WORLD_BEGIN { driver.addNode(); } ;
world_end : WORLD_END { driver.selectParent(); } ;

attribute_begin : ATTRIBUTE_BEGIN { driver.addNode(); } ;
attribute_end : ATTRIBUTE_END { driver.selectParent(); } ;

transform_begin : TRANSFORM_BEGIN { driver.addNode(); } ;
transform_end : TRANSFORM_END { driver.selectParent(); } ;



translate
    : TRANSLATE float float float { driver.addTranslate($2, $3, $4); }
    ;

rotate
    : ROTATE float float float float { driver.addRotate($2, $3, $4, $5); }
    ;

scale
    : SCALE float float float { driver.addScale($2, $3, $4); }
    ;

contcat_transform: CONCAT_TRANSFORM float_array
    {
        driver.addConcatTransform(*$2);
        delete $2;
    } ;

string_array
    : LEFT_SQUARE_BRACKET string_list RIGHT_SQUARE_BRACKET { $$ = $2; }
    ;

string_list
    : string_list STRING { $1->push_back($2); $$ = $1; }
    | STRING { $$ = new std::vector<std::string>; $$->push_back($1); }
    ;

float_array
    : LEFT_SQUARE_BRACKET float_list RIGHT_SQUARE_BRACKET { $$ = $2; }
    ;

float_list
    : float_list float { $1->push_back($2); $$ = $1; }
    | float { $$ = new std::vector<float>; $$->push_back($1); }
    ;

float
    : FLOAT { $$ = $1; }
    | INT { $$ = (float)$1; }
    ;

int_array
    : LEFT_SQUARE_BRACKET int_list RIGHT_SQUARE_BRACKET { $$ = $2; }
    ;

int_list
    : int_list INT { $1->push_back($2); $$ = $1; }
    | INT { $$ = new std::vector<int>; $$->push_back($1); }
    ;


sides : SIDES INT;
orientation : ORIENTATION STRING;
opacity : OPACITY float_array { delete $2; };
color : COLOR float_array { delete $2; };
surface
    : surface STRING float_array { delete $3; }
    | surface STRING float
    | SURFACE STRING
    ;
geometry : GEOMETRY STRING;

light_source
    : light_source STRING float_array { delete $3; }
    | light_source STRING float
    | LIGHT_SOURCE
    ;

option
    : option STRING float_array { delete $3; }
    | option STRING float
    | option INT
    | OPTION STRING
    ;

integrator
    : integrator STRING int_array { delete $3; }
    | INTEGRATOR STRING STRING
    ;

hider
    : hider STRING int_array { delete $3; }
    | hider STRING string_array { delete $3; }
    | HIDER STRING
    ;

projection
    : projection STRING float_array { delete $3; }
    | projection STRING INT
    | PROJECTION STRING
    ;

area_light_source
    : area_light_source STRING float_array { delete $3; }
    | area_light_source STRING string_array { delete $3; }
    | AREA_LIGHT_SOURCE STRING STRING
    ;

version : VERSION FLOAT ;
display : DISPLAY STRING STRING STRING ;
format : FORMAT float float float ;
pixel_samples : PIXEL_SAMPLES INT INT ;
pixel_filter : PIXEL_FILTER STRING INT INT ;
pixel_variance : PIXEL_VARIANCE FLOAT ;
shading_rate : SHADING_RATE float ;
exposure : EXPOSURE FLOAT FLOAT ;
scoped_coordinate_system : SCOPED_COORDINATE_SYSTEM STRING ;

%%

void rib::Parser::error(const rib::Parser::location_type &l,
                        const std::string &err_message)
{
    driver.clean(&driver.root);
    std::cerr << "Error: " << err_message << " at " << l << "\n";
}

