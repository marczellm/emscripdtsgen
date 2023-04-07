#include <fstream>
#include "emscripdtsgen.hpp"

int main () {
#ifdef EXPORT_NAME
    std::string exportName = EXPORT_NAME;
#else 
    std::string exportName = "Module";
#endif

#ifdef EXPORT_ES6
    std::string extension = ".d.mts";
#else
    std::string extension = ".d.ts";
#endif

    std::ofstream outFile (exportName + extension);
    for (const auto& declarable: ModuleContents::Get ().mDeclarations) {
        outFile << std::endl << declarable->declaration () << std::endl;
    }
    outFile << "interface EmscriptenModule2 extends EmscriptenModule {" << std::endl;    
    for (const auto& declarable: ModuleContents::Get ().mDeclarations) {
        if (declarable->export_on_module()) {
            outFile << std::endl << "\t" << *declarable->export_on_module() << std::endl;
        }        
    }
    outFile << std::endl << "}" << std::endl;

#ifdef EXPORT_ES6
    outFile << "export default ";
#else 
    outFile << "declare ";
#endif

#ifdef MODULARIZE
    outFile << "function " << exportName << "(): Promise<EmscriptenModule2>;" << std::endl;
#else
    outFile << "const " << exportName << ": EmscriptenModule2;" << std::endl;
#endif
    outFile.close();
}