//
// Created by Ben on 14/3/2026.
//

#include "WorldView.h"

#include <FXPLM.h>


WorldView::WorldView( int w, int h ){

	m_fboCanvas = new gz_fbo(w,h);

	m_fboCanvas->m_FboClearColorRGBA[0] = 0.2;
	m_fboCanvas->m_FboClearColorRGBA[1] = 0.3;
	m_fboCanvas->m_FboClearColorRGBA[2] = 0.2;

}

WorldView::~WorldView(){
	delete m_fboCanvas;
}


void WorldView::render_world(void *target_fbo, const float fov, const bool dbg_tri, const double dt) {


	gz_fbo* m_fboCanvas = (gz_fbo*)target_fbo;

	// switch to an FBO target so we can
	// render the FXPLM 2D layers
	if( m_fboCanvas ) {
#if 1
		m_fboCanvas->push_fbo();
		glPushMatrix();
		{

			// --- 1. Distinct 3D Drawing Phase ---
			{
				//glEnable(GL_DEPTH_TEST); //this will likely be toggled for skybox so just leave it dead.
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();

				// Simple perspective: FOV, Aspect, Near, Far (Values in meters)
				//FIXME: add drefs for near/far frustum vals?
				gluPerspective(fov, (float)m_fboCanvas->m_width / m_fboCanvas->m_height, 0.1, 1000.0);

				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();


#if 1
				{
					glPushAttrib(GL_ALL_ATTRIB_BITS);
					glPushMatrix(); // Save transformation matrices

					FXPLM_DrawCBS_3D();

					glPopMatrix();
					glPopAttrib();
				}
#endif

				glPopMatrix(); // Restore Modelview
				glMatrixMode(GL_PROJECTION);
				glPopMatrix(); // Restore Projection
				glMatrixMode(GL_MODELVIEW);
				//glDisable(GL_DEPTH_TEST);
			}

			// -- end of 3d drawing setup


			glPushAttrib(GL_ALL_ATTRIB_BITS);

			FXPLM_DrawCBS(); //this will also call for window drawing

			glPopAttrib();


			// --- optional debug triangle 2D ---
			if( dbg_tri ){
				glPushMatrix();
				glTranslatef(128, 128, 0);
				draw_triangle_box(dt);
				glPopMatrix();
			}

		}
		glPopMatrix();
		m_fboCanvas->pop_fbo();
#endif
		//HostApp::gui_Plugins.m_fbo = m_fboCanvas;
	}


}



void WorldView::draw_triangle_box( double dt ){
	/* Render here */

	static float r = 0.f;
	r += (30.0f * dt);

	//we want a 100pix base size on the triangle so its easy to see.
	constexpr float base_size = 200.f;
	constexpr float half = base_size / 2.2f;

	glPushMatrix();

	glTranslatef( half/2, 0, 0 );
	glTranslatef( half/2, half/2, 0 );
	//glTranslatef( -half, -half, 0 );
	glRotatef( r, 0,0,-1 );
	//glRotatef( r*0.75, 0,1,0 );

	glBegin(GL_TRIANGLES);
	glColor3f( 1.f, 0.f, 0.f );
	glVertex3f( -half, -half, 0.f );

	glColor3f( 0.f, 1.f, 0.f );
	glVertex3f( half, -half, 0.f );

	glColor3f( 0.f, 0.f, 1.f );
	glVertex3f( 0.f, half, 0.f );
	glEnd();

	glPopMatrix();

}


