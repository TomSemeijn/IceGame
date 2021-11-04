#ifndef REBOOTDRAWBATCH_H
#define REBOOTDRAWBATCH_H

#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Content.h"

namespace reb {

	//struct for keeping track of all the sprites before they are batched
	struct DrawInfo {

		//source info
		std::vector<sf::Vertex> m_verts;
		sf::FloatRect m_boundingBox;
		Content* m_content = nullptr;
		sf::Texture* m_diffuseTexture = nullptr;
		sf::Texture* m_normalTexture = nullptr;

		//sorting and drawing info
		bool m_shaded;
		bool m_transparent;


		//shading info
		int m_alpha;
		int m_cells;
		float m_rotation;
		bool m_flipX;
		bool m_flipY;
		float m_z;

		//sets the bounding box from the vertices
		void setAutoBoundingBox() {
			if (!m_verts.empty()) {
				sf::VertexArray vertArr{ sf::Quads, m_verts.size() };
				for (int k = 0; k < m_verts.size(); k++) {
					vertArr[k] = m_verts[k];
				}
				m_boundingBox = vertArr.getBounds();
			}
		};

	};

	//a struct that holds the processed vertexArrays after they have been divided into batches
	struct DrawBatch {

		//holds the initial drawInfo objects
		std::vector<DrawInfo> m_infos;

		//the vertex arrays
		sf::VertexArray m_diffuseVerts;
		sf::VertexArray m_normalVerts;
		sf::VertexArray m_infoVerts;

		//returns the diffuse texture used by this batch
		const sf::Texture* getDiffuse() {
			for (auto& info : m_infos) {
				if (info.m_diffuseTexture != nullptr) {
					return info.m_diffuseTexture;
				}
			}
			return nullptr;
		};

		//returns the normal texture used by this batch
		const sf::Texture* getNormal() {
			for (auto& info : m_infos) {
				if (info.m_normalTexture != nullptr) {
					return info.m_normalTexture;
				}
			}
			return nullptr;
		};

	};

	//an actual layer that is drawn to
	struct DrawLayer {

		//the target textures for every stage of rendering
		sf::RenderTexture m_diffuseTarget;
		sf::RenderTexture m_normalTarget;
		sf::RenderTexture m_infoTarget;

		//constructor
		DrawLayer(unsigned int width, unsigned int height) : m_diffuseTarget{ }, m_normalTarget{ }, m_infoTarget{ } {
			m_diffuseTarget.create(width, height);

			m_normalTarget.create(width, height);

			m_infoTarget.create(width, height);
		};

		//updates size if necessary
		void updateSize(unsigned int width, unsigned int height) {
			if (m_diffuseTarget.getSize().x != width || m_diffuseTarget.getSize().y != height) {
				m_diffuseTarget.create(width, height);
			}
			if (m_normalTarget.getSize().x != width || m_normalTarget.getSize().y != height) {
				m_normalTarget.create(width, height);
			}
			if (m_infoTarget.getSize().x != width || m_infoTarget.getSize().y != height) {
				m_infoTarget.create(width, height);
			}
		};

	};

}

#endif