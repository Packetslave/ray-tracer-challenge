# ray-tracer-challenge

Working through the book "[The Ray Tracer Challenge](http://www.raytracerchallenge.com)" by Jamis Buck. 

![test render](https://i.imgur.com/4llS5hm.jpg)

### Status

* all shapes implemented except cylinders
* CSG chapter not implemented yet
* parallel (row-based) rendering implemented using coroutines (folly::coro)
* bounding boxes and (basic) BVH implemented from the bonus chapter
* a few tests missing here or there

### Stuff to do

- [ ] clean up the code / tests
- [ ] basic anti-aliasing (see "Ray Tracing in One Weekend", chapter 6)
- [ ] multiple lights
- [ ] bonus chapter on area lights
- [ ] add MTL file support for materials
- [ ] implement vertex textures from OBJ files

### Maybe

- [ ] bonus chapter on texture mapping
- [ ] use https://github.com/vilya/minipbrt to load a subset of pbrt scene files
- [ ] implement Pixar's USD (universal scene description) format
