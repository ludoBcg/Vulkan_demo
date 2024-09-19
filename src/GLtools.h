/*********************************************************************************************************************
 *
 * GLtools.h
 *
 * Minimal classes for Trackball and Camera
 * 
 * Vulkan_demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/


#ifndef GLTOOLS_H
#define GLTOOLS_H


#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>
#include <algorithm>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace GLtools
{

    /*------------------------------------------------------------------------------------------------------------+
    |                                             MISC. CLASSES                                                   |
    +------------------------------------------------------------------------------------------------------------*/


    /*!
    * \class Trackball
    * \brief Handles trackball interaction
    */
    class Trackball
    {
    private:

        double m_radius = 1.0;                                      /*!< radius */
        bool m_tracking = false;                                    /*!< tracking activated/deactivated boolean state */
        glm::vec2 m_center = glm::vec2(0.0f, 0.0f);                 /*!< 2D center's coords */
        glm::vec3 m_vStart = glm::vec3(0.0f, 0.0f, 1.0f);           /*!< 3D coords sarting position */
        glm::quat m_qStart = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);     /*!< quaternion starting position */
        glm::quat m_qCurrent = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);   /*!< quaternion current rotation */


    public:


        /*!
        * \fn Trackball
        * \brief Default constructor
        */
        Trackball() = default;


        /*!
        * \fn init
        * \brief Initialize trackball
        *
        * \param _width : viewport width
        * \param _height : viewport height
        */
        void init(int _width, int _height)
        {
            m_radius = double((std::min)(_width, _height)) * 0.5f;
            m_center = glm::vec2(_width, _height) * 0.5f;
        }


        /*!
        * \fn reStart
        * \brief Set trackball in initial position
        */
        void reStart()
        {
            m_qCurrent = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }


        /*!
        * \fn mapMousePointToUnitSphere
        * \brief Maps 2D coords in screen space (i.e., mouse pointer) to 3D coords on unit sphere
        *
        * \param _point : 2D coords in screen space
        * \return 3D coords on unit sphere
        */
        glm::vec3 mapMousePointToUnitSphere(glm::vec2 _point)
        {
            // calculate the vector between center and point
            double x = _point[0] - m_center[0];
            double y = -_point[1] + m_center[1];
            double z = 0.0f;

            // the closer point is from center, the greater z is
            if (x * x + y * y < m_radius * m_radius / 2.0f)
            {
                z = std::sqrt(m_radius * m_radius - (x * x + y * y));
            }
            else
            {
                z = (m_radius * m_radius / 2.0f) / std::sqrt(x * x + y * y);
            }

            // normalize vector coords to get a point on unit sphere
            return glm::normalize(glm::vec3(x, y, z));
        }


        /*!
        * \fn startTracking
        * \brief Start trackball tracking from a given point
        */
        void startTracking(glm::vec2 _point)
        {
            m_center = _point; // !! @ 

            m_vStart = mapMousePointToUnitSphere(_point);
            m_qStart = glm::quat(m_qCurrent);
            m_tracking = true;
        }


        /*!
        * \fn stopTracking
        * \brief Stop trackball tracking
        */
        void stopTracking()
        {
            m_tracking = false;
        }


        /*!
        * \fn startTracking
        * \brief Rotate trackball to match a new given position (i.e., mouse movement)
        */
        void move(glm::vec2 _point)
        {
            // get new position
            glm::vec3 vCurrent = mapMousePointToUnitSphere(_point);
            // calculate rotation axis between init and new positions
            glm::vec3 rotationAxis = glm::cross(m_vStart, vCurrent);
            // calculate rotation angle between init and new positions
            float dotProduct = (std::max)((std::min)(glm::dot(m_vStart, vCurrent), 1.0f), -1.0f);
            float rotationAngle = std::acos(dotProduct);

            float eps = 0.01f;
            if (rotationAngle < eps)
            {
                // no rotation is angle is small
                m_qCurrent = glm::quat(m_qStart);
            }
            else
            {
                // Note: here we provide rotationAngle in radians. Older versions
                // of GLM (0.9.3 or earlier) require the angle in degrees.

                // build quaternion from rotation angle and axis
                glm::quat q = glm::angleAxis(rotationAngle, rotationAxis);
                q = glm::normalize(q);
                m_qCurrent = glm::normalize(glm::cross(q, m_qStart));
            }
        }


        /*!
        * \fn getRotationMatrix
        * \brief Get trackball orientation (quaternion) as a rotation matrix.
        */
        glm::mat4 getRotationMatrix()
        {
            return glm::mat4_cast(m_qCurrent);
        }


        /*!
        * \fn isTracking
        * \brief Tracking state getter
        */
        bool isTracking() { return m_tracking; }

    }; // end class Trackball





    /*!
    * \class Camera
    * \brief Handles camera matrices
    */
    class Camera
    {
    private:

        glm::mat4 m_projectionMatrix = glm::mat4(1.0f); /*!< Perspective projection matrix */
        glm::mat4 m_viewMatrix = glm::mat4(1.0f);       /*!< View matrix */

        float m_nearPlane = 0.1f;                       /*!< distance to near clip plane */
        float m_farPlane = 50.0f;                       /*!< distance to far clip plane */
        float m_fovy = 45.0f;                           /*!< field of view angle */
        float m_aspect = 3.0f / 4.0f;                   /*!< aspect ration */
        float m_zoomFactor = 1.0f;                      /*!< factor applied to fov for zoom effect */
        float m_orthoOpening = 1.0f;                    /*!< dimension of window to capture for orthognal projection */


    public:


        /*!
        * \fn Camera
        * \brief Default constructor
        */
        Camera() = default;


        /*!
        * \fn init
        * \brief Initialize camera attributes  and matrices
        *
        * \param _near : distance to near clip plane
        * \param _far : distance to far clip plane
        * \param _fov : field of view angle
        * \param _zoomFactor : factor applied to fov for zoom effect
        * \param _width : viewport width
        * \param _height : viewport height
        * \param _camCoords : 3D coords of the camera position
        * \param _centerCoords : 3D coords of the scene's center (i.e., the position to look at)
        * \param _projType : projection type: perspective = 0, orthogonal = 1
        * \param _radScene : radius of the scene (for orthogonal projection only)
        */
        void init(float _near, float _far, float _fov, float _zoomFactor, int _width, int _height, glm::vec3 _camCoords, glm::vec3 _centerCoords, int _projType, float _radScene = 0.0f)
        {
            m_nearPlane = _near;
            m_farPlane = _far;
            m_fovy = _fov;
            m_orthoOpening = _radScene * 2.0f;

            initProjectionMatrix(_width, _height, _zoomFactor, _projType);
            initViewMatrix(_camCoords, _centerCoords);
        }


        /*!
        * \fn initProjectionMatrix
        * \brief Initialize the perspective projection matrix given the viewport dimensions and zoom factor
        * \param _projType : 3projection type: perspective = 0, orthogonal = 1
        */
        void initProjectionMatrix(int _width, int _height, float _zoomFactor, int _projType)
        {
            m_aspect = (float)_width / (float)_height;
            m_zoomFactor = _zoomFactor;

            if (_projType == 0)
                m_projectionMatrix = glm::perspective(glm::radians(m_fovy) * m_zoomFactor, m_aspect, m_nearPlane, m_farPlane);
            else if (_projType == 1)
                m_projectionMatrix = glm::ortho(-m_orthoOpening, m_orthoOpening, -m_orthoOpening, m_orthoOpening, m_nearPlane, m_farPlane);
            else
                std::cerr << "[WARNING] Camera::initProjectionMatrix(): projection type shuld be either 0 (perspective) or 1 (orthogonal):" << std::endl;
        }


        /*!
        * \fn initViewMatrix
        * \brief Initialize view matrix given the 3D coords of the camera position and the scene's center (i.e., the position to look at)
        */
        void initViewMatrix(glm::vec3 _camCoords, glm::vec3 _centerCoords)
        {
            m_viewMatrix = glm::lookAt(_camCoords, _centerCoords, glm::vec3(0, 1, 0));
        }


        /*!
        * \fn getProjectionMatrix
        * \brief ProjectionMatrix getter
        */
        glm::mat4 getProjectionMatrix() { return m_projectionMatrix; }


        /*!
        * \fn getViewMatrix
        * \brief ViewMatrix getter
        */
        glm::mat4 getViewMatrix() { return m_viewMatrix; }


    }; // end class Camera




    /*------------------------------------------------------------------------------------------------------------+
    |                                            MISC. FUNCTIONS                                                  |
    +------------------------------------------------------------------------------------------------------------*/



    /*!
    * \fn sphericalToEuclidean
    * \brief Spherical coordinates to Euclidean coordinates
    * \param _spherical : spherical 3D coords
    * \return 3D Euclidean coords
    */
    inline glm::vec3 sphericalToEuclidean(glm::vec3 _spherical)
    {
        return glm::vec3(sin(_spherical.x) * cos(_spherical.y),
            sin(_spherical.y),
            cos(_spherical.x) * cos(_spherical.y)) * _spherical.z;
    }



} // namespace GLtools

#endif // GLTOOLS_H