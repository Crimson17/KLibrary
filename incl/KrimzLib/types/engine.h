#pragma once


namespace kl {
	class engine {
	public:
		// Engine properties
		float deltaTime = 0;
		float elapsedTime = 0;
		float gravity = 9.81f;
		kl::color background = kl::constant::colors::gray;
		kl::camera gameCamera = kl::camera();
		
		// Ambient and directional lights
		kl::light ambient = kl::light();
		kl::light directional = kl::light();

		// User defined functions
		std::function<void()> start = []() {};
		std::function<void()> update = []() {};

		// Start a new engine
		void startNew(kl::size frameSize) {
			/* Engine timer */
			kl::time timer = kl::time();

			/* Shader uniforms */
			kl::shaders* objectShaders = nullptr;
			kl::uniform wUni = kl::uniform();
			kl::uniform vpUni = kl::uniform();
			kl::uniform ambientUni = kl::uniform();
			kl::uniform directUni = kl::uniform();
			kl::uniform directDirUni = kl::uniform();

			/* Window start definition */
			gameWindow.start = [&]() {
				/* Setting up the face culling */
				kl::opengl::setCulling(false);

				/* Setting up the depth testing */
				kl::opengl::setDepthTest(true);

				/* Setting up the camera */
				gameCamera.setAspect(frameSize);
				gameCamera.setPlanes(0.01f, 100);
				gameCamera.sensitivity = 0.025f;

				/* Setting up the lights */
				ambient.color = kl::color(255, 255, 255);
				ambient.intensity = 0.1;
				directional.color = kl::color(255, 255, 255);
				directional.intensity = 1;
				directional.direction = kl::vec3(0, -1, 1);

				/* Compiling engine shaders */
				objectShaders = new kl::shaders(
					kl::file::readText("res/shaders/object.vs"),
					kl::file::readText("res/shaders/object.fs")
				);

				/* Getting shader uniforms */
				wUni = objectShaders->getUniform("w");
				vpUni = objectShaders->getUniform("vp");
				ambientUni = objectShaders->getUniform("ambientLight");
				directUni = objectShaders->getUniform("directLight");
				directDirUni = objectShaders->getUniform("directDirec");

				/* Calling the user start */
				start();
			};

			/* Window update definition */
			gameWindow.update = [&]() {
				/* Clearing the buffers */
				kl::opengl::clearBuffers(background);

				/* Time calculations */
				elapsedTime = timer.stopwatchElapsed();
				deltaTime = timer.getElapsed();

				/* Calling the user update */
				update();

				/* Calling the physics update */
				updatePhysics();

				/* Setting the camera uniforms */
				vpUni.setData(gameCamera.matrix());

				/* Setting the light uniforms */
				ambientUni.setData(ambient.getLight());
				directUni.setData(directional.getLight());
				directDirUni.setData(directional.getDirection());

				/* Rendering objects */
				for (objItr = gObjects.begin(); objItr != gObjects.end(); objItr++) {
					if (objItr->visible) {
						wUni.setData(objItr->geometry.matrix());
						objItr->render();
					}
				}

				/* Rendering skybox */


				/* Updating the fps display */
				gameWindow.setTitle(std::to_string(int(1 / deltaTime)));

				/* Swapping the frame buffers */
				gameWindow.swapFrameBuffers();
			};

			/* Window end definition */
			gameWindow.end = [&]() {
				delete objectShaders;
			};

			/* Window creation */
			timer.getElapsed();
			timer.stopwatchReset();
			gameWindow.startNew(frameSize, kl::random::getString(6), false, true, true);
		}
		~engine() {
			stop();
		}
		void stop() {
			gameWindow.stop();
		}

		// Returns a reference to engine window
		kl::window& getWindow() {
			return gameWindow;
		}

		// Creates a new game object
		kl::gameobject* newObject() {
			gObjects.push_back(kl::gameobject());
			return &gObjects.back();
		}

		// Deletes a game object
		bool delObject(kl::gameobject* objectAddress) {
			for (objItr = gObjects.begin(); objItr != gObjects.end(); objItr++) {
				if (&*objItr == objectAddress) {
					gObjects.erase(objItr);
					return true;
				}
			}
			return false;
		}

	private:
		// Window
		kl::window gameWindow = kl::window();

		// Object buffer
		std::list<kl::gameobject> gObjects = {};
		std::list<kl::gameobject>::iterator objItr = {};

		// Computing object physics 
		void updatePhysics() {
			for (objItr = gObjects.begin(); objItr != gObjects.end(); objItr++) {
				if (objItr->physics.enabled) {
					// Applying gravity
					objItr->physics.velocity.y -= gravity * objItr->physics.gravity * deltaTime;

					// Applying velocity
					objItr->geometry.position.x += objItr->physics.velocity.x * deltaTime;
					objItr->geometry.position.y += objItr->physics.velocity.y * deltaTime;
					objItr->geometry.position.z += objItr->physics.velocity.z * deltaTime;

					// Applying angular momentum
					objItr->geometry.rotation.x += objItr->physics.angular.x * deltaTime;
					objItr->geometry.rotation.y += objItr->physics.angular.y * deltaTime;
					objItr->geometry.rotation.z += objItr->physics.angular.z * deltaTime;
				}
			}
		}
	};
}
