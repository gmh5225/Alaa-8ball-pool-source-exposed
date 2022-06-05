#include <iostream>
#include <vector>
#include <string>
#include <d3d9.h>
#include <cfloat>
#include <climits>

#define ctl_clear	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0369, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_read    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0367, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_base    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0368, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


// Requires the targetted window to be active and the foreground window to draw.
#define D2DOV_REQUIRE_FOREGROUND	(1 << 0)

// Draws the FPS of the overlay in the top-right corner
#define D2DOV_DRAW_FPS				(1 << 1)

// Attempts to limit the frametimes so you don't render at 500fps
#define D2DOV_VSYNC					(1 << 2)

// Sets the text font to Calibri
#define D2DOV_FONT_CALIBRI			(1 << 3)

// Sets the text font to Arial
#define D2DOV_FONT_ARIAL			(1 << 4)

// Sets the text font to Courier
#define D2DOV_FONT_COURIER			(1 << 5)

// Sets the text font to Gabriola
#define D2DOV_FONT_GABRIOLA			(1 << 6)

// Sets the text font to Impact
#define D2DOV_FONT_IMPACT			(1 << 7)

// The function you call to set up the above options.  Make sure its called before the DirectOverlaySetup function
void DirectOverlaySetOption(DWORD option);

// typedef for the callback function, where you'll do the drawing.
typedef void(*DirectOverlayCallback)(int width, int height);

// Initializes a the overlay window, and the thread to run it.  Input your callback function.
// Uses the first window in the current process to target.  If you're external, use the next function
void DirectOverlaySetup(DirectOverlayCallback callbackFunction);

// Used to specify the window manually, to be used with externals.
void DirectOverlaySetup(DirectOverlayCallback callbackFunction, HWND targetWindow);

// Draws a line from (x1, y1) to (x2, y2), with a specified thickness.
// Specify the color, and optionally an alpha for the line.
void DrawLine(float x1, float y1, float x2, float y2, float thickness, float r, float g, float b, float a = 1);

// Draws a rectangle on the screen.  Width and height are relative to the coordinates of the box.  
// Use the "filled" bool to make it a solid rectangle; ignore the thickness.
// To just draw the border around the rectangle, specify a thickness and pass "filled" as false.
void DrawBox(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled);

// Draws a circle.  As with the DrawBox, the "filled" bool will make it a solid circle, and thickness is only used when filled=false.
void DrawCircle(float x, float y, float radius, float thickness, float r, float g, float b, float a, bool filled);

// Allows you to draw an elipse.  Same as a circle, except you have two different radii, for width and height.
void DrawEllipse(float x, float y, float width, float height, float thickness, float r, float g, float b, float a, bool filled);

// Draw a string on the screen.  Input is in the form of an std::string.
void DrawString(std::string str, float fontSize, float x, float y, float r, float g, float b, float a = 1);

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float number) const {
		return Vector3(x * number, y * number, z * number);
	}
};

/*The Vector2d class is an object consisting of simply an x and
  y value. Certain operators are overloaded to make it easier
  for vector math to be performed.*/

class Vector2d {
public:
	/*The x and y values are public to give easier access for
	  outside funtions. Accessors and mutators are not really
	  necessary*/
	float x;
	float y;

	//Constructor assigns the inputs to x and y.
	Vector2d();
	Vector2d(float, float);

	/*The following operators simply return Vector2ds that
	  have operations performed on the relative (x, y) values*/
	Vector2d operator+(const Vector2d&) const;
	Vector2d operator-(const Vector2d&) const;
	Vector2d operator*(const Vector2d&) const;
	Vector2d operator/(const Vector2d&) const;

	//Check if the Vectors have the same values.
	bool operator==(const Vector2d&) const;

	/*Check which Vectors are closer or further from the
	  origin.*/
	bool operator>(const Vector2d&) const;
	bool operator<(const Vector2d&) const;
	bool operator>=(const Vector2d&) const;
	bool operator<=(const Vector2d&) const;

	//Negate both the x and y values.
	Vector2d operator-() const;

	//Apply scalar operations.
	Vector2d operator*(const float&) const;
	Vector2d operator/(const float&) const;

	//Product functions
	static float DotProduct(const Vector2d&, const Vector2d&);
	static float CrossProduct(const Vector2d&, const Vector2d&);

	//Returns the length of the vector from the origin.
	static float Magnitude(const Vector2d&);

	//Return the unit vector of the input
	static Vector2d Normal(const Vector2d&);

	//Return a vector perpendicular to the left.
	static Vector2d Perpendicular(const Vector2d&);

	//Return true if two line segments intersect.
	static bool Intersect(const Vector2d&, const Vector2d&, const Vector2d&, const Vector2d&);

	//Return the point where two lines intersect.
	static Vector2d GetIntersect(const Vector2d&, const Vector2d&, const Vector2d&, const Vector2d&);
};

typedef struct info_t {
	int pid = 0;
	UINT_PTR address;
	void* value;
	SIZE_T size;
	void* data;
}info, * p_info;

template <typename Type>
Type read(void* Driver_File, unsigned long int Process_Identifier, unsigned long long int Address)
{
	info_t Input_Output_Data;

	Input_Output_Data.pid = Process_Identifier;

	Input_Output_Data.address = Address;

	Type Return_Value;

	Input_Output_Data.value = &Return_Value;

	Input_Output_Data.size = sizeof Type;

	unsigned long int Readed_Bytes_Amount;

	DeviceIoControl(Driver_File, ctl_read, &Input_Output_Data, sizeof Input_Output_Data, &Input_Output_Data, sizeof Input_Output_Data, &Readed_Bytes_Amount, nullptr);

	return *(Type*)& Return_Value;
}

struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}


enum eBone
{
	BONE_NULL_1 = 0,
	BONE_NULL_2 = 1,
	BONE_PELVIS_1 = 2,
	BONE_PELVIS_2 = 3,
	BONE_PELVIS_3 = 4,
	BONE_TORSO = 5,

	BONE_CHEST_LOW = 6,
	BONE_CHEST = 7,

	// -------------------------

	BONE_CHEST_LEFT = 8,

	BONE_L_SHOULDER_1 = 9,
	BONE_L_ELBOW = 10,

	BONE_L_HAND_ROOT_1 = 11,

	BONE_L_FINGER_1_ROOT = 12,
	BONE_L_FINGER_1_LOW = 13,
	BONE_L_FINGER_1 = 14,
	BONE_L_FINGER_1_TOP = 15,

	BONE_L_FINGER_2_ROOT = 16,
	BONE_L_FINGER_2_LOW = 17,
	BONE_L_FINGER_2 = 18,
	BONE_L_FINGER_2_TOP = 19,

	BONE_L_FINGER_3_ROOT = 20,
	BONE_L_FINGER_3_LOW = 21,
	BONE_L_FINGER_3 = 22,
	BONE_L_FINGER_3_TOP = 23,

	BONE_L_FINGER_4_ROOT = 24,
	BONE_L_FINGER_4_LOW = 25,
	BONE_L_FINGER_4_ = 26,
	BONE_L_FINGER_4_TOP = 27,

	BONE_L_THUMB_ROOT = 28,
	BONE_L_THUMB_LOW = 29,
	BONE_L_THUMB = 30,

	BONE_L_HAND_ROOT_2 = 31,
	BONE_L_WRIST = 32,
	BONE_L_ARM_LOWER = 33,

	BONE_L_SHOULDER_2 = 34,

	BONE_L_ARM_TOP = 35,

	// -------------------------

	BONE_CHEST_TOP_1 = 36,

	// -------------------------

	BONE_CHEST_RIGHT = 37,

	BONE_R_ELBOW = 38,

	BONE_R_HAND_ROOT_1 = 39,

	BONE_R_FINGER_1_ROOT = 40,
	BONE_R_FINGER_1_LOW = 41,
	BONE_R_FINGER_1 = 42,
	BONE_R_FINGER_1_TOP = 43,

	BONE_R_FINGER_2_ROOT = 44,
	BONE_R_FINGER_2_LOW = 45,
	BONE_R_FINGER_2 = 46,
	BONE_R_FINGER_2_TOP = 47,

	BONE_R_FINGER_3_ROOT = 48,
	BONE_R_FINGER_3_LOW = 49,
	BONE_R_FINGER_3 = 50,
	BONE_R_FINGER_3_TOP = 51,

	BONE_R_FINGER_4_ROOT = 52,
	BONE_R_FINGER_4_LOW = 53,
	BONE_R_FINGER_4_ = 54,
	BONE_R_FINGER_4_TOP = 55,

	BONE_R_THUMB_ROOT = 56,
	BONE_R_THUMB_LOW = 57,
	BONE_R_THUMB = 58,

	BONE_R_HAND_ROOT = 59,
	BONE_R_WRIST = 60,
	BONE_R_ARM_LOWER = 61,

	BONE_R_SHOULDER = 62,

	BONE_R_ARM_TOP = 63,

	// -------------------------

	BONE_CHEST_TOP_2 = 64,

	BONE_NECK = 65,
	BONE_HEAD = 66,

	// -------------------------

	BONE_L_LEG_ROOT = 67,
	BONE_L_KNEE = 68,
	BONE_L_FOOT_ROOT = 69,
	BONE_L_SHIN = 70,
	BONE_L_FOOT_MID = 71,
	BONE_L_FOOT_LOW = 72,
	BONE_L_THIGH = 73,

	// -------------------------

	BONE_R_LEG_ROOT = 74,
	BONE_R_KNEE = 75,
	BONE_R_FOOT_ROOT = 76,
	BONE_R_SHIN = 77,
	BONE_R_FOOT_MID = 78,
	BONE_R_FOOT_LOW = 79,
	BONE_R_THIGH = 80,

	// -------------------------

	BONE_NULL_3 = 81,
	BONE_MISC_L_FOOT = 82,
	BONE_MISC_R_FOOT = 83,
	BONE_NULL_4 = 84,
	BONE_MISC_R_HAND_1 = 85,
	BONE_MISC_L_HAND = 86,
	BONE_MISC_R_HAND_2 = 87,
};

double deg2rad(double degrees) {
	return degrees * 4.0 * atan(1.0) / 180.0;
}