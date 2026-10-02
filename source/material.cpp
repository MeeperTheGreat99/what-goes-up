#include "material.h"
#include <map>
#include <fstream>
#include <sstream>

Material::Material() {
    memset(this, 0, sizeof(Material));
}

bool Material::Load(std::string filename, Material& out) {
    std::map<std::string, std::string> entries;
    std::string line;
    std::ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::getline(stream, line, '=');
        std::string key = line;
        std::getline(stream, line);
        std::string value = line;
        entries[key] = value;
    }

    file.close();

    Material mat;
    mat.flags |= MATERIAL_FLAG_ALBEDO_TEXTURE;
    mat.albedo.texture = Texture::Load("res/textures/" + entries["albedoMap"], false, true);

    if (entries.count("normalMap") && !entries["normalMap"].empty()) {
        mat.normal = Texture::Load("res/textures/" + entries["normalMap"], false, true);
    }

    mat.specular.value = entries.count("specular") ? std::stof(entries["specular"]) : 0.5f;
    mat.reflectivity = entries.count("reflectivity") ? std::stof(entries["reflectivity"]) : 0.0f;

    out = mat;

    return true;
}