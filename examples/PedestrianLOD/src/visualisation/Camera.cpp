#include "Camera.h"

#include <glm/gtx/rotate_vector.hpp>

/*
Initialises the camera located at (1,1,1) directed at (0,0,0)
*/
Camera::Camera()
    : Camera(glm::vec3(1, 1, 1))
{}
/*
Initialises the camera located at eye, directed at (0,0,0)
@param eye The coordinates the camera is located
*/
Camera::Camera(glm::vec3 eye)
    : Camera(eye, glm::vec3(0, 0, 0))
{}
//Initialiser list written to remove any references to member variables
//Because member variables are initialised via initaliser lists in the order they are declared in the class declaration (rather than the order of the initialiser list)
/*
Initialises the camera located at eye directed at target
@param eye The coordinates the camera is located
@param target The coordinates the camera is directed towards
*/
Camera::Camera(glm::vec3 eye, glm::vec3 target)
    : pureUp(0.0f, 0.0f, 1.0f)
    , eye(eye)
    , look(normalize(target - eye))
    , right(normalize(cross(target - eye, glm::vec3(0, 0, 1))))
    , up(normalize(cross(cross(target - eye, glm::vec3(0, 0, 1)), target - eye)))
    , stabilise(true)
{
    //this->eye = eye;                                  //Eye is the location passed by user
    //this->look = target - eye;                        //Look is the direction from eye to target
    //this->right = cross(look, pureUp);                //Right is perpendicular to look and pureUp
    //this->up = cross(right, look);                    //Up is perpendicular to right and look

    this->updateViews();
}
/*
Default destructor
*/
Camera::~Camera(){
}
/*
Rotate look and right, yaw radians about up
Rotate look and up, pitch radians about right
@param yaw The number of radians to rotate the camera's direction about the up vector
@param pitch The number of radians to rotate the camera's direction about the right vector
*/
void Camera::turn(float yaw, float pitch){
    //Rotate everything yaw rads about up vector
    this->look = rotate(this->look, -yaw, this->up);
    this->right = rotate(this->right, -yaw, this->up);
    //Rotate everything pitch rads about right vector
    glm::vec3 look = rotate(this->look, -pitch, this->right);
    glm::vec3 up = rotate(this->up, -pitch, this->right);
    glm::vec3 right = this->right;
    if (stabilise)
    {
        //Right is perpendicular to look and pureUp
        right = cross(look, this->pureUp);
        //Stabilised up is perpendicular to right and look
        up = cross(right, look);
        //Don't let look get too close to pure up, else we will spin
        if (abs(dot(look, this->pureUp)) > 0.98)
            return;
    }
    //Commit changes
    this->look = normalize(look);
    this->right = normalize(right);
    this->up = normalize(up);
    this->updateViews();
}
/*
Move eye specified distance along look
@param distance The number of units to move the camera
*/
void Camera::move(float distance){
    eye += look*distance;
    this->updateViews();
}
/*
Rotate right and up, roll radians about look
@param roll The number of radians to rotate the camera's direction about the look vector
*/
void Camera::roll(float roll){
    pureUp = normalize(rotate(pureUp, roll, look));
    right = normalize(rotate(right, roll, look));
    up = normalize(rotate(up, roll, look));
    this->updateViews();
}
/*
Move eye specified distance along right
@param distance The number of units to move the camera
*/
void Camera::strafe(float distance){
    eye += right*distance;
    this->updateViews();
}
/*
Move eye specified distance along up
@param distance The number of units to move the camera
*/
void Camera::ascend(float distance){
    eye += up*distance;
    this->updateViews();
}
/*
Updates the view and skyboxView matrices
Called whenever any internal camera variables are updated
*/
void Camera::updateViews(){
    viewMat = glm::lookAt(eye, eye + look, up);
    skyboxViewMat = glm::lookAt(glm::vec3(0), look, up);
}
/*
Returns the projection matrix
For use with shader uniforms or glLoadMatrixf() after calling glMatrixMode(GL_MODELVIEW)
@return the modelview matrix as calculated by glm::lookAt(glm::vec3, glm::vec3, glm::vec3)
*/
glm::mat4 Camera::view() const{
    return viewMat;
}
/*
Returns the projection matrix from the perspective required for rendering a skybox (direction only)
For use with shader uniforms or glLoadMatrixf() after calling glMatrixMode(GL_MODELVIEW)
@return the modelview matrix as calculated by glm::lookAt(glm::vec3, glm::vec3, glm::vec3)
*/
glm::mat4 Camera::skyboxView() const{
    return skyboxViewMat;
}
/*
Returns the cameras location
@return The location of the camera in world space
*/
glm::vec3 Camera::getEye() const{
    return eye;
}
/*
Returns the cameras normalized direction vector
@return The normalized direction of the camera
*/
glm::vec3 Camera::getLook() const{
    return look;
}
/*
Returns the cameras normalized up vector
@return The normalized direction the camera treats as upwards
*/
glm::vec3 Camera::getUp() const{
    return up;
}
/*
Returns the value of pureUp
This value is used by the stabilisation to prevent the camera rolling unintentionally
@return The normalized direction the camera treats as the true up
*/
glm::vec3 Camera::getPureUp() const{
    return pureUp;
}
/*
Returns the cameras normalized right vector
@return The normalized direction the camera treats as rightwards
*/
glm::vec3 Camera::getRight() const{
    return right;
}
/*
Sets whether the camera should be stabilised
When the camera is stabilised, the up vector will not rotate about the cameras direction
When the camera is not stabilsed, moving the mouse in a circular motion may cause the camera to roll
@param stabilise Whether the camera should be stabilised
*/
void Camera::setStabilise(bool stabilise){
    this->stabilise = stabilise;
}
/*
Returns a constant pointer to the cameras modelview matrix
This pointer can be used to continuously track the modelview matrix
@return A pointer to the modelview matrix
*/
const glm::mat4 *Camera::getViewMatPtr() const{
    return &viewMat;
}
/*
Returns a constant pointer to the cameras skybox modelview matrix
This pointer can be used to continuously track the skybox modelview matrix
@return A pointer to the modelview matrix
*/
const glm::mat4 *Camera::getSkyboxViewMatPtr() const{
    return &skyboxViewMat;
}
