#include "scene.hpp"
#include <chrono>


void Scene::InitLightCDF() {
    light_weights.clear();
    light_cdf.clear();
    total_light_weight = 0.f;

    int num_light = 0;

    for (const auto& light : lights) {
        float weight = light->getArea() * light->radiance.norm();
        light_weights.push_back(weight);
        total_light_weight += weight;
        light_cdf.push_back(total_light_weight);
        light->index = num_light;
        num_light++;
    }

    for (auto& v : light_cdf) { 
        v /= total_light_weight;
    }
}

Lightsampleresult Scene::SampleLights(Intersection inter) const {
    // auto start = std::chrono::high_resolution_clock::now();

    Lightsampleresult result;
    
    if (lights.empty() || total_light_weight == 0.f) {
        result.pdf = 0;
        return result; 
    }
    
    float r1 = get_random_number();

    auto it = std::lower_bound(light_cdf.begin(), light_cdf.end(), r1);
    int index = std::clamp(int(it - light_cdf.begin()), 0, (int)lights.size() - 1);
    
    const auto& light = lights[index];
    float pdf = light_weights[index] / total_light_weight;
    result = light->sample(inter);
    result.pdf *= pdf;

    // auto end = std::chrono::high_resolution_clock::now();
    // time_lightsampling += std::chrono::duration<double>(end - start);

    return result;
}

Vector3d Scene::Shade(const Ray& ray, int depth) const {
    if (depth >= maxDepth) {
        return Vector3d(0.0f); 
    }

    Vector3d L_ind(0.0f), L_dir(0.0f);
    Vector3d wo = -ray.dir;
    float pdf_emitter, pdf_emitter_bsdf, pdf_bsdf, pdf_bsdf_lum;
    
    /* intersection */
    Intersection x_inter = this->intersect(ray);
    Vector3d x_normal = x_inter.normal;
    Point3d x_coord = x_inter.coord;

    /* no intersection, return background color */
    if( !x_inter.intersected ) {
        return Vector3d(0.0f, 0.0f, 0.0f); 
    } 

    // directly hit light source, if depth == 0, return radiance
    if ( x_inter.obj->hasEmit() ) {
        return (depth == 0) ? x_inter.obj->getLight()->radiance : Vector3d(0.0f); 
    }

    Vector2d uv = x_inter.uv;
    Vector3d albedo = x_inter.obj->getMaterial()->getAlbedo(uv);
    float roughness = x_inter.obj->getMaterial()->getRoughness(uv);
    
    /* ---------------- emitter sampling ---------------- */
    Lightsampleresult light_sample = SampleLights(x_inter);
    Vector3d wi_dir = light_sample.dir;
    float dist_dir = light_sample.distance;
    Vector3d radiance_dir = light_sample.radiance;

    if ( radiance_dir.norm() > 0.0f ) {
        /* intersection to judge if the ray is blocked in the middle */
        Ray r_dir(x_coord, wi_dir);
        Intersection light_inter = this->intersect(r_dir);
        Vector3d light_normal = light_inter.normal;

        float cos_theta_light = (-wi_dir).dot(light_normal);
        if (cos_theta_light > 0.0f) {
            pdf_emitter = light_sample.pdf * dist_dir * dist_dir / cos_theta_light;

            if (x_inter.obj->getMaterial()->isdelta) {
                L_dir = 0.0f;
            } else {
                /* direct light source */
                if (std::fabs(light_inter.distance - dist_dir) < 1e-3f * dist_dir ) { // check if the ray is blocked in the middle
                    L_dir =  radiance_dir * x_inter.obj->getMaterial()->computeBSDF(wo, wi_dir, x_normal, albedo, roughness) * 
                        wi_dir.dot(x_normal) / pdf_emitter;
                }

                if (L_dir.norm() > 0.0f) {
                    /* Calculate prob. of sampling that direction using BSDF sampling */
                    pdf_emitter_bsdf = x_inter.obj->getMaterial()->pdf(wo, wi_dir, x_normal, albedo, roughness);

                    /* Weight using the power heuristic */
                    float weight = miWeight(pdf_emitter, pdf_emitter_bsdf);

                    L_dir *= weight;
                }
            }
        }
    }

    /* ---------------- BSDF sampling ---------------- */
    float p = get_random_number(); 
    if (p < RussianRoulette) {
        /* sample BSDF */
        Vector3d wi_bsdf = x_inter.obj->getMaterial()->sample(wo, x_normal, albedo, roughness);
        pdf_bsdf = x_inter.obj->getMaterial()->pdf(wo, wi_bsdf, x_normal, albedo, roughness);
        Vector3d bsdf_val = x_inter.obj->getMaterial()->computeBSDF(wo, wi_bsdf, x_normal, albedo, roughness);
        float cos_theta_x_bsdf = wi_bsdf.dot(x_normal);

        if (cos_theta_x_bsdf > Epsilon && pdf_bsdf > Epsilon) {
            /* Trace a ray in this direction */
            Ray r_bsdf(x_coord, wi_bsdf);
            Intersection bsdf_inter = this->intersect(r_bsdf);
            Vector3d bsdf_normal = bsdf_inter.normal;
            float dist_bsdf = bsdf_inter.distance;

            Vector3d Li(0.0f);
            if (bsdf_inter.intersected) {
                if (bsdf_inter.obj->hasEmit()) {
                    Li = bsdf_inter.obj->getLight()->radiance;
                } else {
                    Li = Shade(r_bsdf, depth + 1) / RussianRoulette; // recursive call
                }
            }

            // if (pdf_bsdf < 1e-4f) {
            //     std::cout << "pdf_bsdf is too small: " << pdf_bsdf << std::endl;
            // }

            L_ind = Li * bsdf_val * cos_theta_x_bsdf / pdf_bsdf;
                
            if (bsdf_inter.intersected && !x_inter.obj->getMaterial()->isdelta && bsdf_inter.obj->hasEmit()) {
                float light_selection_pdf = light_weights[bsdf_inter.obj->getLight()->index] / total_light_weight;

                float cos_theta_light = (-wi_bsdf).dot(bsdf_normal);
                if (cos_theta_light > Epsilon) {
                    /* Compute the prob. of generating that direction using the implemented direct illumination sampling technique */
                    pdf_bsdf_lum = light_selection_pdf * dist_bsdf * dist_bsdf / cos_theta_light
                                    / bsdf_inter.obj->getLight()->getArea();

                    /* Weight using the power heuristic */
                    float weight = miWeight(pdf_bsdf, pdf_bsdf_lum);

                    L_ind *= weight;
                }
            }

        }
    }

    return L_ind + L_dir;
}

