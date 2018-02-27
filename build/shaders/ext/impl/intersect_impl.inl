#pragma once

bool intersect_bounds(
	const vec3 origin,
	const vec3 direction,

	const vec3 bounds_min,
	const vec3 bounds_max,
	const float max_distance)
{
	vec3 inv_direction = 1.f / direction;

	//intersections with box planes parallel to x, y, z axis
	vec3 t135 = (bounds_min - origin) * inv_direction;
	vec3 t246 = (bounds_max - origin) * inv_direction;

	vec3 min_values = min(t135, t246);
	vec3 max_values = max(t135, t246);

	float tmin = max(max(min_values.x, min_values.y), min_values.z);
	float tmax = min(min(max_values.x, max_values.y), max_values.z);

	return tmax >= 0 && tmin <= tmax && tmin <= max_distance;
}

bool intersect_bounds(
	const vec3 origin,
	const vec3 direction,

	const vec3 bounds_min,
	const vec3 bounds_max,
	const float max_distance,
	inout float min_distance)
{
	vec3 inv_direction = 1.f / direction;

	//intersections with box planes parallel to x, y, z axis
	vec3 t135 = (bounds_min - origin) * inv_direction;
	vec3 t246 = (bounds_max - origin) * inv_direction;

	vec3 min_values = min(t135, t246);
	vec3 max_values = max(t135, t246);

	float tmin = max(max(min_values.x, min_values.y), min_values.z);
	float tmax = min(min(max_values.x, max_values.y), max_values.z);

	min_distance = min(tmin, tmax);
	return tmax >= 0 && tmin <= tmax && tmin <= max_distance;
}

bool intersect_triangle(
	const vec3 origin,
	const vec3 direction,
	const vec3 v1,
	const vec3 v2,
	const vec3 v3,
	inout float t,
	inout vec2 barycentric)
{
	float float_epsilon = 1e-23f;
	float border_epsilon = 1e-6f;

	//Find vectors for two edges sharing V1
	vec3 e1 = v2 - v1;
	vec3 e2 = v3 - v1;

	//if determinant is near zero, ray lies in plane of triangle
	vec3 P = cross(vec3(direction), e2);
	float det = dot(e1, P);
	if (det > -float_epsilon && det < float_epsilon)
		return false;

	//Calculate u parameter and test bound
	float inv_det = 1.f / det;
	vec3 T = origin.xyz - v1;
	barycentric.x = dot(T, P) * inv_det;

	//The intersection lies outside of the triangle
	if (barycentric.x < -border_epsilon || barycentric.x > 1.f+border_epsilon) 
		return false;

	//Calculate V parameter and test bound
	vec3 Q = cross(T, e1);
	barycentric.y = dot(vec3(direction), Q) * inv_det;
	//The intersection lies outside of the triangle
	if (barycentric.y < -border_epsilon || barycentric.x + barycentric.y  > 1.f+border_epsilon) 
		return false;

	return (t = dot(e2, Q) * inv_det) > float_epsilon;
}

