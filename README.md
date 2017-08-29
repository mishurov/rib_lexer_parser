RIB lexer & parser
=====
A lexer and a parser for Pixar RenderMan RIB files made with Flex and Bison. It doesn't cover the whole grammar only some geometry: quadric surfaces and points general polygons. It can be extended but I don't need it right now. I made it as a boilerplate for building custom parsers with Flex and Bison in C++.

![ScreenShot1](http://mishurov.usite.pro/github/rib_lexer_parser/teapot.png)

## Info
Visualisation for the parser is implemented as a Maya locator node and uses Viewport 2.0 API. The quadrics are drawn as point clouds: uniformly sampled points along the parameters of the functions since it's the simplest way to visualise the parsed geometry.

The parser returns a syntax tree which is actually a scene tree and the visualiser traverses the tree using a depth first search and an auxiliary stack for the nested transformations. The parser will fail to parse a file if it encounters some unknown tokens or sequences of the tokens not covered in parser's rules, it's only tested with the files in the directory "samples".

The functions for computing point locations for the quadrics are based on these formulas:<br>
https://web.cs.wpi.edu/~matt/courses/cs563/talks/renderman/quadric.html

