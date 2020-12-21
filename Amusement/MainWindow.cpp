#include "MainWindow.h"

MainWindow::MainWindow(int x, int y, int w, int h) :Fl_Gl_Window(x, y, w, h)
{
	this->windowWidth = w;
	this->windowHeight = h;

}

MainWindow::~MainWindow()
{
}

void MainWindow::tick()
{
	this->time += 1.0 / 60.0;


	float speed = 2.0f;
	glm::vec3 front = this->direction;
	front /= glm::length(front);
	glm::vec3 right = glm::cross(front, this->camup);
	right /= glm::length(right);
	if (this->pressedKeys != 0)
	{
		using namespace std;
		//cout << "Pressed: ";
		if (this->pressedKeys & KeyMap::LMB)
		{
			//cout << "LMB, ";
		}
		if (this->pressedKeys & KeyMap::RMB)
		{
			//cout << "RMB, ";
		}
		if (this->pressedKeys & KeyMap::SHIFT)
		{
			//cout << "SHIFT, ";
			speed *= 2;
		}
		if (this->pressedKeys & KeyMap::W)
		{
			//cout << "W, ";
			this->campos += (front * speed)*glm::vec3(1.0, 0.0, 1.0);
		}
		if (this->pressedKeys & KeyMap::A)
		{
			//cout << "A, ";
			this->campos -= (right * speed*.5f)*glm::vec3(1.0, 0.0, 1.0);
		}
		if (this->pressedKeys & KeyMap::S)
		{
			//cout << "S, ";
			this->campos -= (front * speed)*glm::vec3(1.0, 0.0, 1.0);
		}
		if (this->pressedKeys & KeyMap::D)
		{
			//cout << "D, ";
			this->campos += (right * speed*.5f)*glm::vec3(1.0, 0.0, 1.0);
		}
		if (this->pressedKeys & KeyMap::E)
		{
			//cout << "E, ";
		}

		if (this->pressedKeys & KeyMap::CTRL)
		{
			//cout << "CTRL, ";

		}
		if (this->pressedKeys & KeyMap::SPACE)
		{
			//cout << "SPACE, ";
			if (this->jump_time == -1.0f)
			{
				this->jump_time = this->time;
			}
		}
		//cout << endl;
	}
	if (this->jump_time == -1.0f)
	{
		this->campos.y = this->charaHeight;
	}
	else
	{
		if ((this->time - this->jump_time) > this->jump_length)
		{
			this->jump_time = -1.0f;
			this->campos.y = this->charaHeight;
		}
		else
		{
			this->campos.y = (((this->time - this->jump_time)*((this->time - this->jump_time) - this->jump_length)) * -4 / (this->jump_length*this->jump_length) * this->jump_height) + this->charaHeight;
		}

	}
}

void MainWindow::init()
{
	if (gladLoadGL())
	{
		using namespace std;
		this->gladInited = true;

		//Load Shaders
		string shaderFolder = "./shaders/";

		this->shaderSimple = new Shader
		(
			(shaderFolder + "simple.vert").c_str(),
			nullptr,
			nullptr,
			nullptr,
			(shaderFolder + "simple.frag").c_str()
		);
		this->shaderBackground = new Shader
		(
			(shaderFolder + "background.vert").c_str(),
			nullptr,
			nullptr,
			nullptr,
			(shaderFolder + "background.frag").c_str()
		);
		this->shaderSurface = new Shader
		(
			(shaderFolder + "forSurface.vert").c_str(),
			(shaderFolder + "forSurface.tesc").c_str(),
			(shaderFolder + "forSurface.tese").c_str(),
			nullptr,
			(shaderFolder + "forSurface.frag").c_str()
		);
		this->shaderPicking = new Shader
		(
			(shaderFolder + "pickSurface.vert").c_str(),
			nullptr,
			nullptr,
			nullptr,
			(shaderFolder + "pickSurface.frag").c_str()
		);
		this->shaderPostprocess = new Shader
		(
			(shaderFolder + "postProcess.vert").c_str(),
			nullptr,
			nullptr,
			nullptr,
			(shaderFolder + "postProcess.frag").c_str()
		);

		//init Projection and view matrix UBO
		this->projViewUBO = UBO();
		projViewUBO.size = sizeof(glm::mat4) * 2;
		glGenBuffers(1, &(this->projViewUBO.ubo));
		glBindBuffer(GL_UNIFORM_BUFFER, this->projViewUBO.ubo);
		glBufferData(GL_UNIFORM_BUFFER, this->projViewUBO.size, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//init Light UBO
		LightManager::initUBO();
		LightManager::addDirLight(LightManager::DirectionalLight{ glm::vec4(0,-1,0,0),glm::vec4(0.1f,0.1f,0.1f,1.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f) });

		//init environment UBO
		this->environmentUBO = UBO();
		environmentUBO.size = sizeof(float) + sizeof(glm::vec3);
		glGenBuffers(1, &(this->environmentUBO.ubo));
		glBindBuffer(GL_UNIFORM_BUFFER, this->environmentUBO.ubo);
		glBufferData(GL_UNIFORM_BUFFER, this->environmentUBO.size, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//init Textures
		string textureFolder = "./texture/";
		this->backgroundTextureCube = new TextureCube(vector<string>({
			(textureFolder + "background/right.jpg"),
			(textureFolder + "background/left.jpg"),
			(textureFolder + "background/top.jpg"),
			(textureFolder + "background/bottom.jpg"),
			(textureFolder + "background/back.jpg"),
			(textureFolder + "background/front.jpg") }));

		this->floorTexture = new Texture2D(textureFolder + "floor/tiles.jpg");
	}
	else
	{

	}
}

void MainWindow::draw()
{
	if (this->gladInited)
	{
		//Setup
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, this->windowWidth, this->windowHeight);
		this->setProjection();
		LightManager::updateUBO(1);
		this->putEnvironmentOnUBO();
		//Clear
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		//Draw

		//Draw background
		glDepthMask(false);
		this->shaderBackground->Use();
		this->shaderBackground->setInt("u_skybox", 0);
		this->backgroundTextureCube->bind(0);
		this->backgroundBox.draw(this->shaderBackground, glm::mat4());

		this->backgroundTextureCube->unbind(0);
		glDepthMask(true);



		this->shaderSimple->Use();

		this->shaderSimple->setInt("u_texture", 0);
		this->shaderSimple->setBool("u_useTexture", true);
		this->floorTexture->bind(0);
		
		floor.draw(shaderSimple, glm::scale(glm::vec3(2000.0f, 1.0f, 2000.0f)));
		this->shaderSimple->setBool("u_useTexture", false);
		this->floorTexture->unbind(0);

		sphere.draw(shaderSimple, glm::translate(glm::vec3(0.0f, 50.0f, 0.0f))*glm::scale(glm::vec3(30.0f, 30.0f, 30.0f)));
	}
}

int MainWindow::handle(int event)
{
	switch (event)
	{
		//mouse down
	case FL_PUSH:
	{
		int button = Fl::event_button();
		if (button == FL_LEFT_MOUSE)
		{
			this->pressedKeys |= KeyMap::LMB;
		}
		else if (button == FL_RIGHT_MOUSE)
		{
			this->pressedKeys |= KeyMap::RMB;
		}
		break;
	}
	//mouse up
	case FL_RELEASE:
	{
		int button = Fl::event_button();
		if (button == FL_LEFT_MOUSE)
		{
			this->pressedKeys &= ~(KeyMap::LMB);
		}
		else if (button == FL_RIGHT_MOUSE)
		{
			this->pressedKeys &= ~(KeyMap::RMB);
		}
		break;
	}
	//mouse move
	case FL_MOVE:
	{

		if (this->mouseLock)
		{
			SetCursorPos((this->x_root() + this->pixel_w() / 2), (this->y_root() + this->pixel_h() / 2));
			int dx = Fl::event_x() - (this->pixel_w()) / 2;
			int dy = Fl::event_y() - (this->pixel_h()) / 2;
			//std::cout << "Mouse moved dx: " << dx << ", dy: " << dy << std::endl;
			this->camPhi += 0.5f*-dy;
			this->camTheta += 0.5f*-dx;
			glm::mat4 thetaRot = glm::rotate(glm::radians(this->camTheta), glm::vec3(0, 1, 0));
			glm::mat4 phiRot = glm::rotate(glm::radians(this->camPhi), glm::vec3(0, 0, 1));
			this->direction = thetaRot * phiRot* glm::vec4(1, 0, 0, 1);
			this->camup = thetaRot * phiRot* glm::vec4(0, 1, 0, 1);

		}
		break;
	}
	//keybroad down
	case FL_KEYDOWN:
	{
		int key = Fl::event_key();
		if (key == 'w')
		{
			this->pressedKeys |= KeyMap::W;
		}
		else if (key == 'a')
		{
			this->pressedKeys |= KeyMap::A;
		}
		else if (key == 's')
		{
			this->pressedKeys |= KeyMap::S;
		}
		else if (key == 'd')
		{
			this->pressedKeys |= KeyMap::D;
		}
		else if (key == 'e')
		{
			this->pressedKeys |= KeyMap::E;
			if (!this->mouseLock)
			{
				this->mouseLock = true;
				RECT rect{ (this->x_root() + this->pixel_w() / 2) - 10,(this->y_root() + this->pixel_h() / 2) - 10,(this->x_root() + this->pixel_w() / 2) + 10,(this->y_root() + this->pixel_h() / 2) + 10 };
				bool succ = ClipCursor(&rect);
			}
			else
			{
				this->mouseLock = false;
				ClipCursor(NULL);
			}
		}
		else if (key == FL_Shift_L)
		{
			this->pressedKeys |= KeyMap::SHIFT;
		}
		else if (key == FL_Control_L)
		{
			this->pressedKeys |= KeyMap::CTRL;
		}
		else if (key == ' ')
		{
			this->pressedKeys |= KeyMap::SPACE;
		}
		break;
	}
	//keybroad up
	case FL_KEYUP:
	{
		int key = Fl::event_key();
		if (key == 'w')
		{
			this->pressedKeys &= ~(KeyMap::W);
		}
		else if (key == 'a')
		{
			this->pressedKeys &= ~(KeyMap::A);
		}
		else if (key == 's')
		{
			this->pressedKeys &= ~(KeyMap::S);
		}
		else if (key == 'd')
		{
			this->pressedKeys &= ~(KeyMap::D);
		}
		else if (key == 'e')
		{
			this->pressedKeys &= ~(KeyMap::E);
		}
		else if (key == FL_Shift_L)
		{
			this->pressedKeys &= ~(KeyMap::SHIFT);
		}
		else if (key == FL_Control_L)
		{
			this->pressedKeys &= ~(KeyMap::CTRL);
		}
		else if (key == ' ')
		{
			this->pressedKeys &= ~(KeyMap::SPACE);
		}
		break;
	}

	default:
		break;
	}

	return Fl_Gl_Window::handle(event);
}

void MainWindow::setProjection()
{
	this->projectionMatrix = glm::perspective(glm::radians(this->fov), (float)this->windowWidth / this->windowHeight, 0.01f, 2000.0f);
	this->viewMatrix = glm::lookAt(this->campos, this->campos + this->direction, this->camup);

	glBindBuffer(GL_UNIFORM_BUFFER, this->projViewUBO.ubo);


	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &this->projectionMatrix);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &this->viewMatrix);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->projViewUBO.ubo, 0, this->projViewUBO.size);
}

void MainWindow::putEnvironmentOnUBO()
{
	glBindBuffer(GL_UNIFORM_BUFFER, this->environmentUBO.ubo);

	int ptr = 0;
	glBufferSubData(GL_UNIFORM_BUFFER, ptr, sizeof(glm::vec3), &this->campos);
	ptr += sizeof(glm::vec3);
	glBufferSubData(GL_UNIFORM_BUFFER, ptr, sizeof(float), &this->time);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 2, this->environmentUBO.ubo, 0, this->environmentUBO.size);
}

