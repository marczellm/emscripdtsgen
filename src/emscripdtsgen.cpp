#include <fstream>
#include "emscripdtsgen.hpp"

int main() {
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
    outFile << "interface CppObjectHandle {\n\tdelete(): void\n}\n";
    for (const auto& declarable: ModuleContents::Get ().mDeclarations) {
        outFile << std::endl << declarable->declaration () << std::endl;
    }
    outFile << std::endl << "interface EmscriptenModule2 extends EmscriptenModule {";
    for (const auto& constant: ModuleContents::Get ().mConstants) {
        outFile << "\n\t" << constant.mName << ": " << constant.mType();
    }
    for (const auto& func: ModuleContents::Get ().mFunctions) {
        outFile << "\n\t" << func.to_method_signature();
    }
    
    for (const auto& declarable: ModuleContents::Get ().mDeclarations) {
        if (declarable->export_on_module()) {
            outFile << "\n\t" << *declarable->export_on_module();
        }        
    }
    outFile << "\n}\n\n";

#ifdef EXPORT_ES6
    outFile << "export default ";
#else 
    outFile << "declare ";
#endif

#ifdef MODULARIZE
    outFile << "function " << exportName << "(): Promise<EmscriptenModule2>;\n";
#else
    outFile << "const " << exportName << ": EmscriptenModule2;\n";
#endif
    outFile.close();
}