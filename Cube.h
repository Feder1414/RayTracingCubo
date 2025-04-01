#ifndef BOX_H
#define BOX_H
//==============================================================================================
// Implementación de una caja axis-aligned (que puede ser un cubo si sus lados son iguales)
// para ray tracing, basada en el método de los "slabs".
// Inspirado en Ray Tracing in One Weekend de Peter Shirley.
//==============================================================================================

#include "hittable.h"
#include "rtweekend.h"


class box : public hittable
{
public:
    box() {}
    // p0: esquina mínima, p1: esquina máxima; ptr es el material asignado a la caja.
    box(const point3& p0, const point3& p1, shared_ptr<material> ptr) :
        box_min(p0), box_max(p1), mp(ptr) {}

    // La función hit determina si un rayo interseca la caja en el intervalo t ∈ [ray_t.min, ray_t.max].
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        double tmin = ray_t.min;
        double tmax = ray_t.max;
        // Para cada eje (0 = x, 1 = y, 2 = z)
        for (int a = 0; a < 3; a++)
        {
            // Calcular el inverso de la dirección para evitar divisiones repetidas.
            auto invD = 1.0 / r.direction()[a];
            // t0 y t1: tiempos en que el rayo cruza los planos correspondientes a box_min y box_max.
            double t0 = (box_min[a] - r.origin()[a]) * invD;
            double t1 = (box_max[a] - r.origin()[a]) * invD;
            // Si la dirección es negativa, intercambiamos t0 y t1.
            if (invD < 0.0)
                std::swap(t0, t1);
            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmax <= tmin)
                return false;
        }

        rec.t = tmin;
        rec.p = r.at(rec.t);

        // Determinar la normal de la cara impactada.
        // Usamos un pequeño "bias" para determinar en cuál de las caras se encuentra el punto.
        vec3         outward_normal;
        const double bias = 1e-4;
        if (std::fabs(rec.p.x() - box_min.x()) < bias)
            outward_normal = vec3(-1, 0, 0);
        else if (std::fabs(rec.p.x() - box_max.x()) < bias)
            outward_normal = vec3(1, 0, 0);
        else if (std::fabs(rec.p.y() - box_min.y()) < bias)
            outward_normal = vec3(0, -1, 0);
        else if (std::fabs(rec.p.y() - box_max.y()) < bias)
            outward_normal = vec3(0, 1, 0);
        else if (std::fabs(rec.p.z() - box_min.z()) < bias)
            outward_normal = vec3(0, 0, -1);
        else if (std::fabs(rec.p.z() - box_max.z()) < bias)
            outward_normal = vec3(0, 0, 1);
        else
            outward_normal = vec3(0, 0, 0); // Esto no debería ocurrir si el punto está en una cara.

        rec.set_face_normal(r, outward_normal);
        rec.mat = mp;
        return true;
    }

    point3               box_min;
    point3               box_max;
    shared_ptr<material> mp;
};

#endif
