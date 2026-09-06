# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRC_DIRS 
    	"Src"
    	"Src/task"
    	"Src/hardware"
    	"Src/service"
    	"Src/service/http"
    	"Src/util"
    INCLUDE_DIRS 
    	"Inc"
    	"Inc/task"
    	"Inc/hardware"
    	"Inc/service"
    	"Inc/service/http"
    	"Inc/util"
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
