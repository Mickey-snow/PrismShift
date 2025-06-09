// #ifndef IMP_H
// #define IMP_H

// #include "tracer/core.hpp"

// std::shared_ptr<Camera> Get_Camera(){
//   std::shared_ptr<Camera> cam = std::make_shared<Camera>();
//   cam->Set_Position(Point3{20,6,20});
//   cam->Set_Looking_point(Point3{0,1.5,1.9});
//   cam->Set_View_angle_vertical(20);
//   return cam;
// }

// std::shared_ptr<Scene> Get_World(){
//   std::shared_ptr<Scene> world = std::make_shared<Scene>();

//   Attribute grayCol{"rgb", Color{0.15,0.15,0.15}},
//     redCol{"rgb", Color{0.85,0.08,0.08}},
//     greenCol{"rgb", Color{0.08,0.85,0.08}};
//   std::shared_ptr<Material> solidwall =
//   (MaterialFactory::Instance()->GetCreateFn("lambertian"))({grayCol});
//   std::shared_ptr<Material> greenwall =
//   (MaterialFactory::Instance()->GetCreateFn("lambertian"))({greenCol});
//   std::shared_ptr<Material> redwall =
//   (MaterialFactory::Instance()->GetCreateFn("lambertian"))({redCol});

//   std::shared_ptr<Material> roofLight =
//   (MaterialFactory::Instance()->GetCreateFn("pointlight"))({Attribute("rgb",std::array<double,3>{8,8,7.8})});

//   Point3 o(0,0,0),i(1,0,0),j(0,1,0),k(0,0,1);
//   std::shared_ptr<Visible> ground =
//   (ShapeFactory::Instance()->GetCreateFn("plane"))(std::vector<Attribute>{
//       Attribute("position",std::array<Point3,3>{o,i,k}),
//       Attribute("material",solidwall)});
//   std::shared_ptr<Visible> lwall =
//   (ShapeFactory::Instance()->GetCreateFn("plane"))(std::vector<Attribute>{
//       Attribute("position",std::array<Point3,3>{o,i,j}),
//       Attribute("material",redwall)});
//   std::shared_ptr<Visible> rwall =
//   (ShapeFactory::Instance()->GetCreateFn("plane"))(std::vector<Attribute>{
//       Attribute("position",std::array<Point3,3>{o,j,k}),
//       Attribute("material",greenwall)});

//   std::shared_ptr<Visible> light =
//   (ShapeFactory::Instance()->GetCreateFn("parallelogram"))(std::vector<Attribute>{
//       Attribute("position", std::array<Point3,3>{Point3{2.5,7,2.5},
//       Point3{7.5,7,2.5}, Point3{2.5,7,7.5}}), Attribute("material",
//       roofLight)});

//   world->Add(light); world->Add(ground); world->Add(lwall);
//   world->Add(rwall); return world;
// }

// #endif
