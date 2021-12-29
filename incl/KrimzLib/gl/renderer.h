#pragma once


namespace kl {
	namespace gl {
		struct renderer {
			/* Time */
			float deltaT = 0;
			float elapsedT = 0;

			/* View */
			kl::color background = kl::constant::colors::gray;
			kl::gl::camera cam;

			/* Lighting */
			kl::gl::ambient dark;
			kl::gl::direct sun;

			/* User functions */
			std::function<void()> setup = []() {};
			std::function<void(kl::keys*, kl::mouse*)> input = [](kl::keys* k, kl::mouse* m) {};
			std::function<void()> update = []() {};

			// Creates and runs a new engine
			void createNew(kl::ivec2 frameSize) {
				/* Engine timer */
				kl::time timer;

				/* Default shaders */
				kl::gl::shaders* default_sha = nullptr;
				kl::gl::uniform w_uni;
				kl::gl::uniform vp_uni;
				kl::gl::uniform dark_uni;
				kl::gl::uniform sunL_uni;
				kl::gl::uniform sunD_uni;
				kl::gl::uniform sunVP_uni;

				/* Window start definition */
				win.start = [&]() {
					/* Setting up the depth testing */
					kl::gl::setDepthTest(true);

					/* Setting up the camera */
					cam.width = frameSize.x;
					cam.height = frameSize.y;
					cam.nearPlane = 0.01;
					cam.farPlane = 100;
					cam.sens = 0.025;
					cam.shadowD = 15;

					/* Setting up the lights */
					dark.color = kl::constant::colors::white;
					dark.intensity = 0.1;
					sun.color = kl::constant::colors::white;
					sun.intensity = 1;
					sun.direction = kl::vec3(0, -1, -2);

					/* Compiling object shaders */
					default_sha = new kl::gl::shaders(
						kl::file::read("res/shaders/gl/default.vert"),
						kl::file::read("res/shaders/gl/default.frag")
					);

					/* Getting object shader uniforms */
					w_uni = default_sha->getUniform("w");
					vp_uni = default_sha->getUniform("vp");
					dark_uni = default_sha->getUniform("dark");
					sunL_uni = default_sha->getUniform("sunL");
					sunD_uni = default_sha->getUniform("sunD");
					sunVP_uni = default_sha->getUniform("sunVP");
					default_sha->getUniform("shadowMap").setData(1);

					/* Generating sun buffers */
					sun.genBuff(4096);

					/* Calling the user start */
					setup();
				};

				/* Window update definition */
				win.update = [&]() {
					/* Time calculations */
					deltaT = timer.getElapsed();
					elapsedT = timer.stopwatchElapsed();

					/* Calling the user input */
					input(&win.keys, &win.mouse);

					/* Calling the user update */
					update();

					/* Calling the physics update */
					for (int i = 0; i < objects.size(); i++) {
						objects[i]->upPhys(deltaT);
					}

					/* Setting the camera uniforms */
					vp_uni.setData(cam.matrix());

					/* Calculating the light vp matrix */
					sun.calcMat(cam);

					/* Setting the light uniforms */
					dark_uni.setData(dark.getCol());
					sunL_uni.setData(sun.getCol());
					sunD_uni.setData(sun.getDir());
					sunVP_uni.setData(sun.matrix());

					/* Rendering the shadows */
					sun.render(frameSize, [&]() {
						for (int i = 0; i < objects.size(); i++) {
							if (objects[i]->shadows) {
								// Setting the world matrix
								sun.setWMat(objects[i]->geometry.matrix());

								// Rendering the object
								objects[i]->render();
							}
						}
						});

					/* Clearing the default buffers */
					kl::gl::clearBuffers(background);

					/* Rendering objects */
					for (int i = 0; i < objects.size(); i++) {
						if (objects[i]->visible) {
							// Setting the world matrix
							w_uni.setData(objects[i]->geometry.matrix());

							// Rendering the object
							objects[i]->render();
						}
					}

					/* Rendering skybox */
					if (sky) sky->render(cam.matrix());

					/* Updating the fps display */
					win.setTitle(std::to_string(int(1 / deltaT)));

					/* Swapping the frame buffers */
					win.swapFrames();
				};

				/* Window end definition */
				win.end = [&]() {
					// Deleting shaders
					delete default_sha;

					// Deleting the sun shadow buffers
					sun.delBuff();

					// Deleting meshes
					for (int i = 0; i < meshes.size(); i++) {
						delete meshes[i];
					}
					meshes.clear();

					// Deleting textures
					for (int i = 0; i < textures.size(); i++) {
						delete textures[i];
					}
					textures.clear();
				};

				/* Window creation */
				timer.getElapsed();
				timer.stopwatchReset();
				win.startNew(frameSize, kl::random::getString(6), false, true, true);
			}
			void stop() {
				win.stop();
			}

			// Returns the visible triangle count
			int triangles() {
				int trCount = 0;
				for (int i = 0; i < objects.size(); i++) {
					if (objects[i]->visible) {
						trCount += objects[i]->mesh->triangles();
					}
				}
				return trCount;
			}

			// Returns the frame center
			kl::ivec2 frameCenter() {
				return win.getCenter();
			}

			// Creates a new skybox
			void newSkybox(kl::image& front, kl::image& back, kl::image& left, kl::image& right, kl::image& top, kl::image& bottom) {
				delSkybox();
				sky = new kl::gl::skybox(front, back, left, right, top, bottom);
			}
			void newSkybox(kl::image&& front, kl::image&& back, kl::image&& left, kl::image&& right, kl::image&& top, kl::image&& bottom) {
				delSkybox();
				sky = new kl::gl::skybox(front, back, left, right, top, bottom);
			}

			// Deletes an existing skybox
			void delSkybox() {
				if (sky) {
					delete sky;
					sky = nullptr;
				}
			}

			// Creates a mesh
			kl::gl::mesh* newMesh(std::string filePath, bool flipZ = true) {
				meshes.push_back(new kl::gl::mesh(filePath, flipZ));
				return meshes.back();
			}
			kl::gl::mesh* newMesh(std::vector<kl::vertex>& vertexData) {
				meshes.push_back(new kl::gl::mesh(vertexData));
				return meshes.back();
			}

			// Deletes a mesh
			bool delMesh(kl::gl::mesh* mesAddress) {
				for (int i = 0; i < meshes.size(); i++) {
					if (meshes[i] == mesAddress) {
						delete meshes[i];
						meshes.erase(meshes.begin() + i);
						return true;
					}
				}
				return false;
			}

			// Creates a texture
			kl::gl::texture* newTexture(kl::image& image) {
				textures.push_back(new kl::gl::texture(image));
				return textures.back();
			}
			kl::gl::texture* newTexture(kl::image&& image) {
				textures.push_back(new kl::gl::texture(image));
				return textures.back();
			}

			// Deletes a texture
			bool delTex(kl::gl::texture* texAddress) {
				for (int i = 0; i < textures.size(); i++) {
					if (textures[i] == texAddress) {
						delete textures[i];
						textures.erase(textures.begin() + i);
						return true;
					}
				}
				return false;
			}

			// Creates a new game object
			kl::gl::renderable* newObject(kl::gl::mesh* mes, kl::gl::texture* tex) {
				objects.push_back(new kl::gl::renderable(mes, tex));
				return objects.back();
			}

			// Deletes a game object
			bool delObject(kl::gl::renderable* objectAddress) {
				for (int i = 0; i < objects.size(); i++) {
					if (objects[i] == objectAddress) {
						delete objects[i];
						objects.erase(objects.begin() + i);
						return true;
					}
				}
				return false;
			}

		private:
			// Window
			kl::window win;

			// Engine skybox
			kl::gl::skybox* sky = nullptr;

			// Mesh buffer
			std::vector<kl::gl::mesh*> meshes;

			// Texture buffer
			std::vector<kl::gl::texture*> textures;

			// Object buffer
			std::vector<kl::gl::renderable*> objects;
		};
	}
}