#include "Renderer.h"
#include "ParticleEmitter.h"
#include "Drawable.h"

namespace reb {

	//creates initial draw layers
	void Renderer::createInitialLayers(sf::RenderTarget& window) {
		//creates initial render layers
		for (int k = 0; k < INITIALLAYERS; k++) {
			m_drawLayers.push_back(new DrawLayer(window.getSize().x, window.getSize().y));
		}
	}

	//renders
	void Renderer::render(sf::RenderTarget& window, const GUI& gui) {
		
		//clears the current toDraw vector
		m_toDraw.clear();
		
		//gets the visible world area
		sf::Vector2f viewSize(window.getView().getSize());
		sf::FloatRect viewBox(window.getView().getCenter() - viewSize / 2.f, viewSize);
		
		//sets the uniforms of the shader
		m_shader->setUniforms(m_scene, window, viewBox);
		
		auto scenePointer = m_scene;
		
		//keeps a vector of draw commands
		std::map<LayerIndex, std::vector<Container*>> draws{};

		auto newDraw = [&draws](LayerIndex layer, Container* container) {
			if (draws.find(layer) == draws.end()) {
				draws.insert(std::make_pair(layer, std::vector<Container*>{}));
			}
			draws.find(layer)->second.push_back(container);
		};
		
		//loops through all objects in the scene
		auto& objects = m_scene->getObjects();
		for (auto& obj : objects) {
			newDraw(obj->getLayer(), obj);
		}
		
		//loops through all layers the scene has tiles in
		auto tileLayers = m_scene->getTilesetLayers();
		for (auto& currentLayer : tileLayers) {
			//gets the set
			auto& set = scenePointer->getConstTileset(currentLayer);

			//gets the coord bounds for tiles within the viewbox
			int minX = (int)std::floorf(viewBox.left / set.TILEWIDTH);
			int minY = (int)std::floorf(viewBox.top / set.TILEHEIGHT);
			int maxX = (int)std::ceilf((viewBox.left + viewBox.width) / set.TILEWIDTH);
			int maxY = (int)std::ceilf((viewBox.top + viewBox.height) / set.TILEHEIGHT);

			//draws all tiles within the viewbox
			set.foreachTile([this, &newDraw, &currentLayer](Tile*const& tile) {
				newDraw(currentLayer, tile);
			}, { minX, maxX }, { minY, maxY });

		}

		//gets the window size
		sf::Vector2u windowSize = window.getSize();
		
		//loops through all the containers
		for (auto& drawLayer : draws) {

			//draws bottom particles if necessary
			if (DRAWPARTICLES) {
				for (auto& container : drawLayer.second) {
					if (auto emit = container->getEmitter()) {
						if (emit->PARTICLERENDERMODE == ParticleEmitter::ParticleRenderMode::PARTICLE_RENDER_BOTTOM) {
							emit->draw(*this, viewBox);
						}
					}
				}
			}

			//draws sprite if necessary
			if (DRAWSPRITES) {
				for (auto& container : drawLayer.second) {
					if (auto spr = container->getSprite()) {
						spr->draw(*this, viewBox, windowSize);
					}
				}
			}

			//draws bottom particles if necessary
			if (DRAWPARTICLES) {
				for (auto& container : drawLayer.second) {
					if (auto emit = container->getEmitter()) {
						if (emit->PARTICLERENDERMODE == ParticleEmitter::ParticleRenderMode::PARTICLE_RENDER_TOP) {
							emit->draw(*this, viewBox);
						}
					}
				}
			}

		}
		
		//distributes the drawInfo's into batches
		std::vector<DrawBatch> layers{};
		layers.emplace_back();
		for (DrawInfo& draw : m_toDraw) {

			if (draw.m_verts.empty() || !viewBox.intersects(draw.m_boundingBox)) { continue; }

			//sets initial layer to layer zero
			int layerIndex = 0;

			//goes through the layers backwards to find the highest intersection if any exist
			if (layers.size() > 1) {

				for (int k = layers.size() - 1; k >= 0; k--) {
					DrawBatch& layer = layers.at(k);
					bool finished = false;
					auto lastInfo = layer.m_infos.end();
					for (auto otherDraw = layer.m_infos.begin(); otherDraw != lastInfo; otherDraw++) {
						//checks for an intersection
						if (otherDraw->m_boundingBox.intersects(draw.m_boundingBox)) {
							//if the current sprite doesnt use transparency, the layer of the intersecting sprite
							//is used to draw on
							if (!draw.m_transparent) {
								layerIndex = k;
								finished = true;
							}
							else if(draw.m_shaded || otherDraw->m_shaded){
								layerIndex = k + 1;
								finished = true;
							}
							else if(otherDraw + 1 != lastInfo){
								for (auto newOtherDraw = otherDraw + 1; newOtherDraw != layer.m_infos.end(); newOtherDraw++) {
									if (newOtherDraw->m_shaded && newOtherDraw->m_boundingBox.intersects(draw.m_boundingBox)) {
										layerIndex = k + 1;
										finished = true;
										break;
									}
								}
							}
							else {
								layerIndex = k;
								finished = true;
							}
							if (finished) { break; }
						}
					}
					//breaks loop if finished
					if (finished) { break; }
				}
			}
			//goes through all sprites of the first layer if there is only one layer and this sprite has transparency
			else if (draw.m_transparent) {
				for (auto& other : layers.front().m_infos) {
					if (other.m_boundingBox.intersects(draw.m_boundingBox)) {
						layerIndex = 1;
						break;
					}
				}
			}

			//gets the sprite batch to add to (adds a new one if necessary)
			DrawBatch* batch = &(layerIndex == layers.size() ? layers.emplace_back() : layers.at(layerIndex));

			while (!batch->m_infos.empty() && batch->getDiffuse() != draw.m_diffuseTexture) {
				layerIndex++;
				batch = &(layerIndex == layers.size() ? layers.emplace_back() : layers.at(layerIndex));
			}

			//puts the sprite in the batch of the found layer
			batch->m_infos.push_back(draw);

		}
		
		//processes the sprites in the batches to fill the vertexArrays
		for (DrawBatch& batch : layers) {
			
			//gets the amount of vertices
			int vertCount = 0;
			for (DrawInfo& info : batch.m_infos) {
				vertCount += info.m_verts.size();
			}

			//creates the vertexArrays
			batch.m_diffuseVerts = sf::VertexArray(sf::PrimitiveType::Quads, vertCount);
			batch.m_normalVerts = sf::VertexArray(sf::PrimitiveType::Quads, vertCount);
			batch.m_infoVerts = sf::VertexArray(sf::PrimitiveType::Quads, vertCount);

			//populates the vertexArrays
			int currentVert = 0;
			for (DrawInfo& info : batch.m_infos) {

				for (auto& vert : info.m_verts) {
					//sets initial vertices to copies of the one in the draw info
					batch.m_diffuseVerts[currentVert] = vert;
					batch.m_normalVerts[currentVert] = vert;
					batch.m_infoVerts[currentVert] = vert;

					//sets info color if shaded
					if (info.m_shaded) {
						//gets the flipped state number
						int flippedState = 64;
						if (info.m_flipX) {
							if (info.m_flipY) {
								flippedState = 255;
							}
							else {
								flippedState = 127;
							}
						}
						else if (info.m_flipY) {
							flippedState = 191;
						}

						batch.m_infoVerts[currentVert].color = sf::Color(
							std::max(std::min(info.m_alpha, 255), 0), 
							flippedState, 
							(int)std::floorf(255.0f * (info.m_rotation / (2.0f * PI))), 
							(int)std::max((int)std::min(info.m_cells, 255), 1)
						);
					}//sets to not shaded otherwise
					else {
						batch.m_infoVerts[currentVert].color = sf::Color(0, 0, 0, 0);
					}

					//sets the alpha value in the color of the normal to match the base z-position
					if (info.m_shaded) {
						batch.m_normalVerts[currentVert].color = sf::Color(255, 255, 255, (int)std::floorf((info.m_z + 255.0f) / 2.0));
					}

					//increments current vertex index
					currentVert++;
				}
			}

		}

		sf::View realView = window.getView();

		//draws every diffuse layer
		int index = 0;
		for (DrawBatch& batch : layers) {
			//adds a new drawLayer if necessary
			if (index == m_drawLayers.size()) {
				m_drawLayers.push_back(new DrawLayer(window.getSize().x, window.getSize().y));
			}
			//gets the drawLayer
			DrawLayer& drawLayer = *m_drawLayers[index];

			//updates the size of the drawLayer if necessary
			drawLayer.updateSize(window.getSize().x, window.getSize().y);

			//draws diffuse
			drawLayer.m_diffuseTarget.clear(sf::Color(0, 0, 0, 0));
			drawLayer.m_diffuseTarget.setView(window.getView());
			const sf::Texture& diffuse = *batch.getDiffuse();
			drawLayer.m_diffuseTarget.draw(batch.m_diffuseVerts, &diffuse);
			drawLayer.m_diffuseTarget.display();
			//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
			//if (layers.size() > m_drawLayers.size()) {
				//drawLayer.m_diffuseTarget.getTexture().copyToImage().saveToFile("Content/0 temp/diffuse " + std::to_string(index) + ".png");
			//}
			//}
			index++;
		}
		//draws every normal layer
		 {
			sf::BlendMode blend = sf::BlendNone;
			sf::Transform transf{};
			index = 0;
			for (auto& batch : layers) {
				sf::RenderStates state{ blend, transf, batch.getNormal(), &m_normalShader };
				const sf::Texture* diffuseTexture = batch.getDiffuse();
				if (diffuseTexture != nullptr) {
					m_normalShader.setUniform("diffuse", *diffuseTexture);
				}

				//gets the drawLayer
				DrawLayer& drawLayer = *m_drawLayers[index];

				//draws normal
				drawLayer.m_normalTarget.clear(sf::Color(0, 0, 0, 0));
				drawLayer.m_normalTarget.setView(realView);
				drawLayer.m_normalTarget.draw(batch.m_normalVerts, state);
				drawLayer.m_normalTarget.display();
				//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
					//drawLayer.m_normalTarget.getTexture().copyToImage().saveToFile("Content/0 temp/normal " + std::to_string(index) + ".png");
				//}
				index++;
			}
		}
		//draws every info layer
		{
			sf::BlendMode blend = sf::BlendNone;
			sf::Transform transf{};
			index = 0;
			for (auto& batch : layers) {
				const sf::Texture* diffuse = batch.getDiffuse();
				sf::RenderStates state{ blend, transf, diffuse, &m_infoShader };

				//gets the drawLayer
				DrawLayer& drawLayer = *m_drawLayers[index];

				//draws info
				drawLayer.m_infoTarget.clear(sf::Color(0, 0, 0, 0));
				drawLayer.m_infoTarget.setView(window.getView());
				drawLayer.m_infoTarget.draw(batch.m_infoVerts, state);
				drawLayer.m_infoTarget.display();
				//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
					//drawLayer.m_infoTarget.getTexture().copyToImage().saveToFile("Content/0 temp/Info " + std::to_string(index) + ".png");
				//}
				index++;
			}
		}
		
		//draws the final layers on the window through the shader
		for (int k = 0; k < layers.size(); k++) {
			m_shader->drawLayer(*m_drawLayers.at(k), window);
		}

		//draws the rigidbodies if necessary
		if (DRAWRIGIDBODIES) {
			for (auto& layer : tileLayers) {
				scenePointer->getConstTileset(layer).foreachTile([&window](Tile* tile) {
					if (auto body = tile->getBody()) {
						body->getShape()->draw(window, *tile->getTransform(), body->SOLID);
					}
				});
			}
			for (auto& obj : objects) {
				if (auto body = obj->getBody()) {
					body->getShape()->draw(window, *obj->getTransform(), body->SOLID);
				}
			}
		}

		//draws the gui
		gui.draw(window, *m_shader);
	
	}

	//draws the given sfml sprite without shading
	void Renderer::draw(sf::Sprite& spr, Content* content) {
		
		//creates the drawInfo
		DrawInfo& info = m_toDraw.emplace_back();

		//gets the world positions of the sprite corners
		auto trans = spr.getTransform();
		auto local = spr.getLocalBounds();
		sf::Vector2f topLeft = trans * sf::Vector2f(local.left, local.top);
		sf::Vector2f topRight = trans * sf::Vector2f(local.left + local.width, local.top);
		sf::Vector2f bottomLeft = trans * sf::Vector2f(local.left, local.top + local.height);
		sf::Vector2f bottomRight = trans * sf::Vector2(local.left + local.width, local.top + local.height);

		//adds the vertex array
		auto color = spr.getColor();
		auto texRect = spr.getTextureRect();
		info.m_verts.emplace_back(topLeft, color, sf::Vector2f(texRect.left, texRect.top));
		info.m_verts.emplace_back(topRight, color, sf::Vector2f(texRect.left + texRect.width, texRect.top));
		info.m_verts.emplace_back(bottomRight, color, sf::Vector2f(texRect.left + texRect.width, texRect.top + texRect.height));
		info.m_verts.emplace_back(bottomLeft, color, sf::Vector2f(texRect.left, texRect.top + texRect.height));

		//sets shaded to false
		info.m_shaded = false;

		//sets the bounding box
		info.m_boundingBox = spr.getGlobalBounds();

		//sets the transparency boolean
		info.m_transparent = content->hasTransparency() || color.a < 255;

		info.m_content = content;
		info.m_diffuseTexture = const_cast<sf::Texture*>(spr.getTexture());
		info.m_normalTexture = const_cast<sf::Texture*>(&content->getNormalTexture());

	}

	//draws the given sfml sprite with shading
	void Renderer::draw(sf::Sprite& spr, Content* content, int alpha, int cells, float baseZ) {

		//creates the drawInfo
		DrawInfo& info = m_toDraw.emplace_back();

		//gets the world positions of the sprite corners
		auto trans = spr.getTransform();
		auto local = spr.getLocalBounds();
		sf::Vector2f topLeft = trans * sf::Vector2f(local.left, local.top);
		sf::Vector2f topRight = trans * sf::Vector2f(local.left + local.width, local.top);
		sf::Vector2f bottomLeft = trans * sf::Vector2f(local.left, local.top + local.height);
		sf::Vector2f bottomRight = trans * sf::Vector2(local.left + local.width, local.top + local.height);

		//adds the vertex array
		auto color = spr.getColor();
		auto texRect = spr.getTextureRect();
		info.m_verts.emplace_back(topLeft, color, sf::Vector2f(texRect.left, texRect.top));
		info.m_verts.emplace_back(topRight, color, sf::Vector2f(texRect.left + texRect.width, texRect.top));
		info.m_verts.emplace_back(bottomRight, color, sf::Vector2f(texRect.left + texRect.width, texRect.top + texRect.height));
		info.m_verts.emplace_back(bottomLeft, color, sf::Vector2f(texRect.left, texRect.top + texRect.height));

		//sets shaded to true
		info.m_shaded = true;

		//sets the bounding box
		info.m_boundingBox = spr.getGlobalBounds();

		//sets the transparency boolean
		info.m_transparent = content->hasTransparency() || color.a < 255;

		//sets the shading information
		info.m_cells = cells;
		info.m_alpha = alpha;
		info.m_flipX = spr.getScale().x < 0;
		info.m_flipY = spr.getScale().y < 0;
		info.m_rotation = getRadians(spr.getRotation());
		info.m_z = baseZ;
		
		info.m_content = content;
		info.m_diffuseTexture = const_cast<sf::Texture*>(spr.getTexture());
		info.m_normalTexture = const_cast<sf::Texture*>(&content->getNormalTexture());

	}

	//draws the given drawInfo
	void Renderer::draw(DrawInfo& toDraw) {
		m_toDraw.push_back(toDraw);
	}

	//destructor
	Renderer::~Renderer() {
		writeLn(m_drawLayers.size() - INITIALLAYERS, " draw layers were added to the initial ", INITIALLAYERS, " during the game");
		if (m_shader != nullptr) {
			delete m_shader;
		}
		for (auto& layer : m_drawLayers) {
			delete layer;
		}
	}

}