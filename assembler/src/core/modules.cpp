#include <bdc_assembler/core/modules.hpp>
#include <bdc_assembler/utils/log.hpp>
#include <bdc_assembler/utils/config.hpp>
#include <bdc_assembler/core/parser.hpp>
#include <filesystem>
#include <dlfcn.h>

static std::vector<module> s_modules;

static void remove_modules(std::vector<module*>& modules_to_remove)
{
    for(uint i = 0; i < s_modules.size(); i++)
        for(uint j = 0; j < modules_to_remove.size(); j++)
            if(s_modules[i].uid == modules_to_remove[j]->uid)
            {
                if(s_modules[i].lib_handle != nullptr)
                    dlclose(s_modules[i].lib_handle);
                s_modules.erase(s_modules.begin() + i);
                i--;
            }
    modules_to_remove.clear();
}
void load_modules()
{
    uint uid_count = 0;
    if(!std::filesystem::exists(MODULES_DIR))
        std::filesystem::create_directory(MODULES_DIR);
    for(auto& entry : std::filesystem::directory_iterator(MODULES_DIR))
        if(entry.is_directory() && std::filesystem::exists(entry.path().string().append("/plateform.pcf")))
        {
            s_modules.push_back(module{.uid = uid_count, .config = read_config_file(entry.path().string().append("/plateform.pcf")), .module_folder = entry.path().string()});
            uid_count++;
        }
    
    std::vector<module*> modules_to_remove;

    for(module& m : s_modules)
    {
        if(m.config.contains("plateform"))
            m.plateform = m.config["plateform"];
        else
        {
            WARN_M("Plateform not specified for module: " << m.module_folder);
            modules_to_remove.push_back(&m);
        }
        if(m.config.contains("lib_file"))
            m.lib_file = m.config["lib_file"];
        else
            m.lib_file = m.plateform;
    }
    remove_modules(modules_to_remove);
    for(module& m : s_modules)
        for(module& m1 : s_modules)
            if(m1.plateform == m.plateform && m.uid != m1.uid)
            {
                modules_to_remove.push_back(&m);
                modules_to_remove.push_back(&m1);
                WARN_M("Found modules targetting the same plateform removing both");
            }
    remove_modules(modules_to_remove);
    for(module& m : s_modules)
    {
        std::stringstream file_path_stream;
        file_path_stream << m.module_folder << "/" << m.lib_file;
#ifdef __linux__
        file_path_stream << ".linux.so"; 
#endif
#ifdef __WIN32
        file_path_stream << ".win.so";
#endif
#ifdef __APPLE__
        file_path_stream << ".apple.so";
#endif
        m.lib_handle = dlopen(file_path_stream.str().c_str(), RTLD_LAZY);
        if(m.lib_handle != nullptr)
        {
            dlerror(); // Make sure that there is no error waiting to be displayed
            m.fp_get_raw_bytes = (std::vector<char> (*)(const std::vector<line_data>&))(dlsym(m.lib_handle, "module_get_raw_bytes"));
            if(dlerror() != nullptr)
            {
                WARN_M("Cannot load module: " << m.module_folder << ". Can't load module_get_raw_bytes function.");
                modules_to_remove.push_back(&m);   
            }
        }
        else
        {
            WARN_M("Cannot load module " << m.module_folder << ". Cannot load the library file");
            ERR_M(dlerror());
            modules_to_remove.push_back(&m);
        }
    }
    remove_modules(modules_to_remove);
}
std::vector<char> call_module(const std::string& plateform, const std::vector<line_data>& v)
{
    DEBUG_M("Looking for module: " << plateform);
    for(module& m : s_modules)
        if(m.plateform == plateform)
            return m.fp_get_raw_bytes(v);
    ERR_M("Module not found for plateform: " << plateform);
    exit(EXIT_FAILURE);
    return std::vector<char>();
}
void unload_modules()
{
    for(module& m : s_modules)
        dlclose(m.lib_handle);
    s_modules.clear();
}