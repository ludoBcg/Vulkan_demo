/*********************************************************************************************************************
 *
 * GLtools.h
 *
 * Toolkit for Graphics applications, contains:
 *      - Minimal classes for Trackball, Camera, Timers
 *      - Helper functions (e.g., read shader files)
 *      - LogStream
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
#include <algorithm>
#include <source_location>
#include <chrono>


#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#ifdef USE_VULKAN
#include <vulkan/vk_enum_string_helper.h>
#endif

#ifdef USE_CUDA
#include <cuda_runtime.h>
#endif

#ifdef USE_OPENGL
#define QT_NO_OPENGL_ES_2
#include <GL/glew.h>
#endif


namespace GLtools
{

    /*------------------------------------------------------------------------------------------------------------+
    |                                               TRACKBALL                                                     |
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



    /*------------------------------------------------------------------------------------------------------------+
    |                                                 CAMERA                                                      |
    +------------------------------------------------------------------------------------------------------------*/


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
        * \brief Initialize the perspective projection matrix given the viewport dimensions and a zoom factor
        * \param _projType : 3projection type: perspective = 0, orthogonal = 1
        */
        void initProjectionMatrix(int _width, int _height, float _zoomFactor, int _projType)
        {
            m_aspect = (float)_width / (float)_height;
            m_zoomFactor = _zoomFactor;

            if (_projType == 0)
                m_projectionMatrix = glm::perspective(glm::radians(m_fovy) * m_zoomFactor, m_aspect, m_nearPlane, m_farPlane);
            else if (_projType == 1)
            {
                //m_projectionMatrix = glm::ortho(-m_orthoOpening * m_aspect, m_orthoOpening * m_aspect, -m_orthoOpening, m_orthoOpening, m_nearPlane, m_farPlane);
                // multiply width by aspect ratio to avoid stretching
                m_projectionMatrix = glm::ortho(-m_orthoOpening * m_aspect * m_zoomFactor, 
                                                 m_orthoOpening * m_aspect * m_zoomFactor, 
                                                -m_orthoOpening * m_zoomFactor, 
                                                 m_orthoOpening * m_zoomFactor, 
                                                 m_nearPlane, 
                                                 m_farPlane);
            }                
            else
                std::cerr << "[WARNING] Camera::initProjectionMatrix(): projection type shuld be either 0 (perspective) or 1 (orthogonal):" << std::endl;
        }


        /*!
        * \fn initViewMatrix
        * \brief Initialize view matrix given the 3D coords of the camera position and the scene's center (i.e., the position to look at)
        */
        void initViewMatrix(glm::vec3 _camCoords, glm::vec3 _centerCoords)
        {
            //m_viewMatrix = glm::lookAt(_camCoords, _centerCoords, glm::vec3(0, 1, 0));
            // define up direction vector
            glm::vec3 upVec = glm::vec3(0, 1, 0);
            // avoid up vector and cam position to be aligned
            if (_camCoords.x == 0 && _camCoords.z == 0)
                upVec = glm::vec3(0, 0, 1);

            m_viewMatrix = glm::lookAt(_camCoords, _centerCoords, upVec);
        }


        /*!
        * \fn getProjectionMatrix
        * \brief ProjectionMatrix getter
        */
        glm::mat4 getProjectionMatrix() const { return m_projectionMatrix; }


        /*!
        * \fn getViewMatrix
        * \brief ViewMatrix getter
        */
        glm::mat4 getViewMatrix() const { return m_viewMatrix; }

        /*!
        * \fn getZoomFactor
        * \brief ZoomFactor getter
        */
        //float getZoomFactor() const { return m_zoomFactor; }

        /*!
        * \fn setZoomFactor
        * \brief ZoomFactor setter
        */
        //void setZoomFactor(const float _zoomFactor) { m_zoomFactor = _zoomFactor; }


    }; // end class Camera




    /*------------------------------------------------------------------------------------------------------------+
    |                                                LOGGER                                                       |
    +------------------------------------------------------------------------------------------------------------*/


    // Defines 3 levels of severity for log messages
    enum LogSeverity 
    {
        INFO = 0,
        WARNING = 1,
        CRITICAL = 2
    };


    /*!
    * \class LogStream
    * \brief Writes to a log file
    *        Uses C++20 std::source_location
    *        cf https://en.cppreference.com/w/cpp/utility/source_location
    */
    class LogStream
    {

        public:

            LogStream(LogSeverity _severity, std::source_location _location)
                : m_severity(_severity), m_location(_location) 
            {
                // only critical messages are written in file
                if (_severity == CRITICAL) {
                    m_file.open("error_log.txt");
                }
            }

            LogStream() = delete;

            ~LogStream()
            {
                if(m_file.is_open())
                    m_file.close();
            }

            std::string logLocation(const std::source_location& _location)
            {
                std::ostringstream stream;
                stream << "file: "
                       << _location.file_name() << "\n\t"
                       << "in function " << _location.function_name() << "(), line " << _location.line() << ", col " << _location.column() << "\n\t";
                return stream.str();
            }

            std::string logMsg(const std::string_view& _message)
            {
                std::ostringstream stream;
                stream << _message << std::endl;
                return stream.str();
            }

            std::ostream& operator<< (std::string const& _msg)
            {
                switch (m_severity)
                {
                    case INFO:      // prints info message only in terminal
                        return std::cout << "[info] " << logMsg(_msg);
                        break;
                    case WARNING:   // prints warning message and location in terminal
                        return std::cout << "[Warning] " << logLocation(m_location) << logMsg(_msg);
                        break;
                    case CRITICAL:  // prints error message and location in terminal and writes in LogFile
                        if (m_file.is_open())
                            m_file << "[ERROR] " << logLocation(m_location) << logMsg(_msg);
                        return std::cerr << "[ERROR] " << logLocation(m_location) << logMsg(_msg);
                        break;
                    default:
                        return std::cerr << "[UNKNOWN] " << logLocation(m_location) << logMsg(_msg);
                        break;
                }
            }

            std::ostream& lastGLerror()
            {
                #ifdef USE_OPENGL
                auto error = glGetError();

                switch (error)
                {
                    case GL_NO_ERROR:
                        if(m_severity == INFO)
                            return std::cout << "[info] " << logMsg("GL no error");
                        else
                            return std::cout << "";
                        break;
                    case GL_INVALID_ENUM:
                        return std::cerr << "[ERROR] " << logLocation(m_location) << logMsg("GL invalid enum");
                        break;
                    case GL_INVALID_VALUE:
                        return std::cerr << "[ERROR] " << logLocation(m_location) << logMsg("GL invalid value");
                        break;
                    case GL_INVALID_OPERATION:
                        return std::cerr << "[ERROR] " << logLocation(m_location) << logMsg("GL invalid operation");
                        break;
                    case GL_INVALID_FRAMEBUFFER_OPERATION:
                        return std::cerr << "[ERROR] " << logLocation(m_location) << logMsg("GL invalid framebuffer operation");
                        break;
                    case GL_OUT_OF_MEMORY:
                        return std::cerr << "[ERROR] " << logLocation(m_location) << logMsg("GL out of memory");
                        break;
                    case GL_STACK_UNDERFLOW:
                        return std::cerr << "[ERROR] " << logLocation(m_location) << logMsg("GL stack underflow");
                        break;
                    case GL_STACK_OVERFLOW:
                        return std::cerr << "[ERROR] " << logLocation(m_location) << logMsg("GL stack overflow");
                        break;
                    default:
                        return std::cerr << "[ERROR] " << logLocation(m_location) << logMsg("GL unknown error");
                        break;
                }
                #endif

                #ifdef USE_CUDA
                    return std::cerr << "[CUDA ERROR] " << logLocation(m_location) << logMsg( cudaGetErrorString(cudaGetLastError()) );
                #endif

            }

            #ifdef USE_VULKAN
            std::ostream& operator<< (VkResult _result)
            {
                if (_result != VK_SUCCESS ) 
                {
                     return std::cerr << "[VULKAN ERROR] " << logLocation(m_location) << logMsg( string_VkResult(_result) );
                }
                else
                {
                    return std::cout << "[info] " << logMsg("VkSuccess");
                }
            }
            #endif

            #ifdef USE_CUDA
            std::ostream& operator<< (cudaError_t _cudaErr)
            {
                if (_cudaErr != cudaSuccess) 
                {
                     return std::cerr << "[CUDA ERROR] " << logLocation(m_location) << logMsg(cudaGetErrorString(_cudaErr));
                }
                else
                {
                    return std::cout << "[info] " << logMsg("cudaSuccess");
                }
            }
            #endif

        protected:

            LogSeverity m_severity;             /*!< Message severity */
            std::source_location m_location;    /*!< Source code location of log */
            std::ofstream m_file;               /*!< LogFile writing stream */

    }; // end class LogStream


    class Logger
    {
        // use Singleton pattern
        protected:

            // Hide constructors in protected field to prevent the class being instanciated from outside 
            Logger() = default;

            virtual ~Logger() = default;

        public:

            // Prevent copy/assignation
            Logger(Logger const& _other) = delete;
            Logger(Logger&& _other) = delete;
            Logger& operator=(Logger const& _other) = delete;
            Logger& operator=(Logger&& _other) = delete;


            LogStream operator()(LogSeverity _severity, const std::source_location location = std::source_location::current()) const
            {
                return LogStream(_severity, location);
            }

            // Static function to create/access instance
            static Logger& instance()
            {
                // static variable is not destroyed at the end of the function
                // will still exist at next call
                static Logger theInstance;
                return theInstance;
            }

    }; // end class Logger


    #define infoLog() GLtools::Logger::instance() (GLtools::INFO)
    #define warningLog() GLtools::Logger::instance() (GLtools::WARNING)
    #define errorLog() GLtools::Logger::instance() (GLtools::CRITICAL)



    /*------------------------------------------------------------------------------------------------------------+
    |                                                LOGGER                                                       |
    +------------------------------------------------------------------------------------------------------------*/


    /*!
    * \struct CpuTimer
    * \brief Host timer using on std::chrono
    */
    struct CpuTimer
    {
        std::chrono::time_point<std::chrono::system_clock> tStart;

        void start(const  std::string& msg)
        {
            std::cout << std::endl << "[Host timer] Starting " << msg << " ..." << std::endl;
            tStart = std::chrono::system_clock::now();
        }

        void stop()
        {
            auto tStop = std::chrono::system_clock::now();
            std::cout << "[Host timer] ... finished in " 
                      << std::chrono::duration_cast<std::chrono::milliseconds>(tStop - tStart).count() 
                      << " ms" << std::endl;
        }
    };


    #ifdef USE_CUDA
    /*!
    * \struct GpuTimer
    * \brief Device timer using cudaEvent_t
    */
    struct GpuTimer
    {
        // uses CUDA events to measure time without cudaDeviceSynchronize()
        // cf. https://developer.nvidia.com/blog/how-implement-performance-metrics-cuda-cc/
        cudaEvent_t tStart, tStop;

        GpuTimer()
        {
            cudaEventCreate(&tStart);
            cudaEventCreate(&tStop);
        }

        void start(const  std::string& msg)
        {
            std::cout << std::endl << "[Device timer] Starting " << msg << " ..." << std::endl;
            cudaEventRecord(tStart);
        }

        void stop()
        {
            cudaEventRecord(tStop);
            cudaEventSynchronize(tStop);
            float milliseconds = 0;
            cudaEventElapsedTime(&milliseconds, tStart, tStop);
            std::cout << "[Device timer] ... finished in " 
                      << milliseconds 
                      << " ms" << std::endl;
        }
    };
    #endif


    /*------------------------------------------------------------------------------------------------------------+
    |                                            MISC. FUNCTIONS                                                  |
    +------------------------------------------------------------------------------------------------------------*/


    /*!
    * \fn readFile
    * \brief Read shader files
    *        Shader file can be either compiled (.spv) or ASCII text (e.g., .vert/.frag/.txt ...etc)
    * \param _filename : path and name of shader files 
    * \return shader code as char array
    */
    inline static std::vector<char> readFile(const std::string& _filename)
    {
        // openmode set to read only
        std::ios_base::openmode readFlag = std::ios::in | std::ios::ate;

        // if file is compiled, open in binary mode
        if (_filename.find(".spv") != std::string::npos)
            readFlag |= std::ios::binary;

        std::ifstream file(_filename, readFlag /*std::ios::ate | std::ios::binary*/);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file! Check if relative path to file is consistent with working directory");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }


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