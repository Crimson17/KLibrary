#include "KrimzLib/KrimzLib.hpp"


kl::window win;

kl::gpu* gpu = nullptr;

kl::shaders* raytracingSha = nullptr;

kl::mesh* screenMes = nullptr;

kl::timer timer;
float deltaT = 0.0f;

kl::camera camera;

kl::sphere spheres[6] = {};

int selected = -1;

struct PS_SP {
	// Geometry
	kl::vec3 center;
	float radius = 0;

	// Light
	kl::vec3 color;
	float reflectivity = 0;
	kl::vec4 emission = 0;
};

struct PS_CB {
	kl::vec4 frameSize;
	kl::mat4 invCam;
	kl::vec4 camPos;
	PS_SP spheres[6] = {};
};

void Start() {
	// Window maximize
	win.maximize();

	// Gpu creation
	gpu = new kl::gpu(win.getWND());

	// Disabling the depth testing
	gpu->setDSState(kl::dbuffer::Disabled);

	// Shader creation
	raytracingSha = gpu->newShaders("res/shaders/raytracing.hlsl", 0, sizeof(PS_CB));
	raytracingSha->bind();

	// Screen mesh creation
	screenMes = gpu->newMesh({
		kl::vertex(kl::vec3(-1.0f, -1.0f, 0.5f)), kl::vertex(kl::vec3(-1.0f, 1.0f, 0.5f)), kl::vertex(kl::vec3(1.0f, 1.0f, 0.5f)),
		kl::vertex(kl::vec3(-1.0f, -1.0f, 0.5f)), kl::vertex(kl::vec3(1.0f, -1.0f, 0.5f)), kl::vertex(kl::vec3(1.0f, 1.0f, 0.5f))
	});

	// Sphere setup
	spheres[0] = kl::sphere(kl::vec3( 0.0f, -5004.0f, 20.0f), 5000.0f, kl::color( 50,  50,  50), 0.0f, 0.0f);
	spheres[1] = kl::sphere(kl::vec3( 0.0f,     0.0f, 20.0f),    4.0f, kl::color(255,  85,  90), 0.8f, 0.0f);
	spheres[2] = kl::sphere(kl::vec3(-5.0f,    -1.0f, 15.0f),    2.0f, kl::color(230, 195, 115), 0.9f, 0.0f);
	spheres[3] = kl::sphere(kl::vec3(-5.0f,     0.0f, 25.0f),    3.0f, kl::color(165, 195, 245), 0.9f, 0.0f);
	spheres[4] = kl::sphere(kl::vec3( 5.5f,     0.0f, 15.0f),    3.0f, kl::color( 30, 230, 230), 0.9f, 0.0f);
	spheres[5] = kl::sphere(kl::vec3( 0.0f,    20.0f, 30.0f),    0.25f, kl::color(255, 255, 255), 0.0f, 3.0f);
}

void Input() {
	// Selection
	if (win.keys.num0) selected = -1;
	if (win.keys.num1) selected =  0;
	if (win.keys.num2) selected =  1;
	if (win.keys.num3) selected =  2;
	if (win.keys.num4) selected =  3;
	if (win.keys.num5) selected =  4;
	if (win.keys.num6) selected =  5;

	// Speed
	if (win.keys.shift) {
		camera.speed = 5;
	}
	else {
		camera.speed = 2;
	}

	// Movement
	if (selected == -1) {
		if (win.keys.w) camera.moveForward(deltaT);
		if (win.keys.s) camera.moveBack(deltaT);
		if (win.keys.d) camera.moveRight(deltaT);
		if (win.keys.a) camera.moveLeft(deltaT);
		if (win.keys.space) camera.moveUp(deltaT);
		if (win.keys.c) camera.moveDown(deltaT);
	}
	else {
		if (win.keys.w) spheres[selected].center += camera.getForward() * (camera.speed * deltaT);
		if (win.keys.s) spheres[selected].center -= camera.getForward() * (camera.speed * deltaT);
		if (win.keys.d) spheres[selected].center += camera.getRight() * (camera.speed * deltaT);
		if (win.keys.a) spheres[selected].center -= camera.getRight() * (camera.speed * deltaT);
		if (win.keys.space) spheres[selected].center += kl::vec3::pos_y * (camera.speed * deltaT);
		if (win.keys.c) spheres[selected].center -= kl::vec3::pos_y * (camera.speed * deltaT);
	}

	// Camera rotation
	static bool camMoving = false;
	if (win.mouse.lmb) {
		// Getting the frame center
		const kl::ivec2 frameCenter = win.getCenter();

		// Fixing the camera jump on the first click
		if (!camMoving) {
			win.mouse.position = frameCenter;
		}

		// Saving info
		win.mouse.hide();
		camMoving = true;

		// Updating the cam
		camera.rotate(win.mouse.position, frameCenter);
		win.mouse.move(frameCenter);
	}
	else {
		// Saving info
		win.mouse.show();
		camMoving = false;
	}
}

void Update() {
	// Time
	deltaT = timer.interval();

	// Input
	Input();

	// Setting data
	PS_CB psData = {};
	psData.frameSize = kl::vec4(win.getSize(), 0.0f, 0.0f);
	psData.invCam = camera.matrix().inverse();
	psData.camPos = camera.position;
	for (int i = 0; i < 6; i++) {
		psData.spheres[i].center = spheres[i].center;
		psData.spheres[i].radius = spheres[i].radius;
		psData.spheres[i].color = spheres[i].color;
		psData.spheres[i].reflectivity = spheres[i].reflectivity;
		psData.spheres[i].emission = spheres[i].calcEmiss();
	}
	raytracingSha->setPixlData(&psData);

	// Raytracing
	screenMes->draw();

	// Buffer swap
	gpu->swap(true);

	// Fps display
	win.setTitle(std::to_string(int(1 / deltaT)));
}

void End() {
	delete gpu;
}

void Resize(const kl::ivec2& newSize) {
	if (gpu && newSize.x > 0 && newSize.y > 0) {
		gpu->regenBuffers(newSize);
		gpu->setDSState(kl::dbuffer::Disabled);
		gpu->setViewport(kl::ivec2(0, 0), newSize);
		camera.aspect = float(newSize.x) / newSize.y;
	}
}

int main() {
	win.start = Start;
	win.update = Update;
	win.end = End;
	win.resize = Resize;
	timer.elapsed();
	win.startNew(kl::ivec2(1600, 900), "Raytracing", true, true);
}
