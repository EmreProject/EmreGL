#include "oglMeshes.h"


namespace OglObject3D {

	//ATTRIBUTES
	SSBOAttribute::SSBOAttribute(GLuint ssbo, int binding) {

		this->ssbo = ssbo;
		this->binding = binding;

	}

	void SSBOAttribute::setSSBO(GLuint ssbo, int binding) {
		this->ssbo = ssbo;
		this->binding = binding;
	}	

	void SSBOAttribute::Use() {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo);
	}




	BufferAttribute::BufferAttribute(){}

	InstancedAttribute::InstancedAttribute():BufferAttribute(){}

	string BufferAttribute::getAttributeNameForHook() {

		string type;


		if (this->value_type == "float") {

		if (count == 1) { type = "float"; }
		if (count == 2) { type = "vec2"; }
		if (count == 3) { type = "vec3"; }
		if (count == 4) { type = "vec4"; }
		}


		if (this->value_type == "int") {

			if (count == 1) { type = "int"; }
			if (count == 2) { type = "ivec2"; }
			if (count == 3) { type = "ivec3"; }
			if (count == 4) { type = "ivec4"; }
		}
		
		if (this->value_type == "uint") {

			if (count == 1) { type = "uint"; }
			if (count == 2) { type = "uvec2"; }
			if (count == 3) { type = "uvec3"; }
			if (count == 4) { type = "uvec4"; }
		}


		ostringstream oss;
		oss << "layout (location = " << this->layout_ << ") in " << type <<" "<< shaderName << ";";
		return oss.str();


	}



	//GEOMETRY

	Geometry::Geometry() {
		position.layout_ = 0;
		position.count = 3;
		position.value_type = "float"; //default is float
		position.shaderName = "position";

		texcoord.layout_ = 1;
		texcoord.count = 2;
		texcoord.value_type = "float"; //default is float
		texcoord.shaderName = "uv";

		normal.layout_ = 2;
		normal.count = 3;
		normal.value_type = "float"; //default is float
		normal.shaderName = "normal";

		tangent.layout_ = 3;
		tangent.count = 4;
		tangent.value_type = "float"; //default is float
		tangent.shaderName = "tangent";

	}

	Geometry::Geometry(const Geometry& other) {
		if (this != &other) {
			this->indices = other.indices;
			this->position = other.position;
			this->texcoord = other.texcoord;
			this->normal = other.normal;
			this->tangent = other.tangent;
			this->bufferAttributes = other.bufferAttributes;
			this->ssboAttributes = other.ssboAttributes;
		

		}

	}

	Geometry& Geometry::operator=(const Geometry& other) {

		if (this != &other) {
			this->indices = other.indices;
			this->position = other.position;
			this->texcoord = other.texcoord;
			this->normal = other.normal;
			this->tangent = other.tangent;
			this->bufferAttributes = other.bufferAttributes;
			this->ssboAttributes = other.ssboAttributes;


		}

		return *this;

	}


	void Geometry::setAttribute(string shaderName, BufferAttribute attribute) {

		function<bool(int layoutToBeChecked)> isLayoutFree = [this](int layoutToBeChecked)->bool {

			bool isFree = true;

			if(bufferAttributes.size()>0){
				auto it = bufferAttributes.begin();
			
				for (; it != bufferAttributes.end(); it++) {
					if (it->layout_ == layoutToBeChecked) {
						isFree = false;
					}
				}
			
			}

			if (instancedBufferAttributes.size() > 0) {

				auto it = instancedBufferAttributes.begin();
				for (; it != instancedBufferAttributes.end(); it++) {

					if (it->layout_ == layoutToBeChecked) {
						isFree = false;
					}
				}

			}
			

			if (layoutToBeChecked == position.layout_ ||
				layoutToBeChecked == texcoord.layout_ ||
				layoutToBeChecked == normal.layout_ ||
				layoutToBeChecked == tangent.layout_) {
				isFree = false;
			}

			return isFree;
			};

		int layout = -1;//start from 0
		bool isFree = false;
		while (!isFree) {
			layout++;
			isFree = isLayoutFree(layout);
		}

		attribute.layout_ = layout;
		attribute.shaderName = shaderName;
		this->bufferAttributes.insert(bufferAttributes.end(), attribute);


	}

	void Geometry::setAttribute(string shaderName, int layout, BufferAttribute attribute) {

		attribute.layout_ = layout;
		attribute.shaderName = shaderName;
		this->bufferAttributes.insert(bufferAttributes.end(), attribute);
	}

	void Geometry::setInstancedAttribute(string shaderName, InstancedAttribute attribute) {

		function<bool(int layoutToBeChecked)> isLayoutFree = [this](int layoutToBeChecked)->bool {

			bool isFree = true;

			if (bufferAttributes.size() > 0) {
				auto it = bufferAttributes.begin();

				for (; it != bufferAttributes.end(); it++) {
					if (it->layout_ == layoutToBeChecked) {
						isFree = false;
					}
				}

			}

			if (instancedBufferAttributes.size() > 0) {

				auto it = instancedBufferAttributes.begin();
				for (; it != instancedBufferAttributes.end(); it++) {

					if (it->layout_ == layoutToBeChecked) {
						isFree = false;
					}
				}

			}


			if (layoutToBeChecked == position.layout_ ||
				layoutToBeChecked == texcoord.layout_ ||
				layoutToBeChecked == normal.layout_ ||
				layoutToBeChecked == tangent.layout_) {
				isFree = false;
			}

			return isFree;
			};

		int layout = -1;//start from 0
		bool isFree = false;
		while (!isFree) {
			layout++;
			isFree = isLayoutFree(layout);
		}

		attribute.layout_ = layout;
		attribute.shaderName = shaderName;
		this->instancedBufferAttributes.insert(instancedBufferAttributes.end(), attribute);


	}
	void Geometry::setInstancedAttribute(string shaderName, int layout, InstancedAttribute attribute) {
		attribute.layout_ = layout;
		attribute.shaderName = shaderName;
		this->instancedBufferAttributes.insert(instancedBufferAttributes.end(), attribute);

	}

	void Geometry::addSSBOAttribute(SSBOAttribute attribute) {


		this->ssboAttributes.insert(this->ssboAttributes.end(), attribute);
	}

	list<BufferAttribute> Geometry::getExtraBufferAttributes() {

		return this->bufferAttributes;
	}

	list<InstancedAttribute> Geometry::getInstancedBufferAttributes() {

		return this->instancedBufferAttributes;

	}

	list<SSBOAttribute> Geometry::getExtraSSBOAttributes() {


		return this->ssboAttributes;
	}

	int Geometry::getBufferNumber() {

		return 3 + this->bufferAttributes.size() + this->instancedBufferAttributes.size();

	}

	bool Geometry::isAttributeExist(string shaderName) {

		bool ret = false;
		for_each(this->bufferAttributes.begin(), this->bufferAttributes.end(), [shaderName,&ret](BufferAttribute& attribute) {
			if (attribute.shaderName == shaderName) {
				ret= true;
			}
			});
		return ret;
	}

	void Geometry::sortAttributes() {

		bufferAttributes.sort([](BufferAttribute& a, BufferAttribute& b) {

			return a.layout_ < b.layout_;

			});


	}

	
	//Buffer Geometry

	BufferGeometry::BufferGeometry():Geometry(){
		

	}

	BufferGeometry::BufferGeometry(const BufferGeometry& other) {
		if (this != &other) {
			Geometry::operator=(other);
		}
	

	}

	BufferGeometry& BufferGeometry::operator=(const BufferGeometry& other) {

		if (this != &other) {
			Geometry::operator=(other);
		}

		return *this;

	}

	
	 BufferGeometry* BufferGeometry::clone() {

		return new BufferGeometry(*this);
	}


	 GeometryType BufferGeometry::GetGeometryType() {
		 return GeometryType::BufferGeometry;
	 }

	//PLANE GEOMETRY
	 PlaneGeometry::PlaneGeometry(const PlaneGeometry& other) {

		 if (this != &other) {
			 Geometry::operator=(other);
		 }
	 }

	 PlaneGeometry& PlaneGeometry::operator=(const PlaneGeometry& other) {

		 if( this != &other ) {
			 Geometry::operator=(other);
		 }
		 return *this;
	 }


	void PlaneGeometry::GeneratePlane(float width, float height, int widthSubdivisions, int heightSubdivisions) {

		

		position.values.erase(position.values.begin(), position.values.end());
		texcoord.values.erase(texcoord.values.begin(), texcoord.values.end());
		normal.values.erase(normal.values.begin(), normal.values.end());

		float xStep = width / widthSubdivisions;
		float yStep = height / heightSubdivisions;
		float halfWidth = width/2;
		float halfHeight = height/2;


		

		// Generate vertices
		for (int j = 0; j <= heightSubdivisions; ++j) {
			for (int i = 0; i <= widthSubdivisions; ++i) {
				float x = -halfWidth + i * xStep;
				float y = -halfHeight + j * yStep;
				float z = 0.0f;  // Plane lies on Z = 0

				// Normal
				float nx = 0.0f, ny = 0.0f, nz = 1.0f;

				// Texture Coordinate
				float u = static_cast<float>(i) / widthSubdivisions;
				float v = static_cast<float>(j) / heightSubdivisions;

				// Push vertex position
				position.values.push_back(x);
				position.values.push_back(y);
				position.values.push_back(z);

				// Push vertex tex coords
				texcoord.values.push_back(u);
				texcoord.values.push_back(v);

				// Push vertex normal
				normal.values.push_back(nx);
				normal.values.push_back(ny);
				normal.values.push_back(nz);
			}
		}

		// Generate indices
		for (int j = 0; j < heightSubdivisions; ++j) {
			for (int i = 0; i < widthSubdivisions; ++i) {
				int row1 = j * (widthSubdivisions + 1);
				int row2 = (j + 1) * (widthSubdivisions + 1);

				// Two triangles per square
				indices.push_back(row1 + i);       // Bottom-left
				indices.push_back(row1 + i + 1);   // Bottom-right
				indices.push_back(row2 + i + 1);   // Top-right

				indices.push_back(row1 + i);       // Bottom-left
				indices.push_back(row2 + i + 1);   // Top-right
				indices.push_back(row2 + i);       // Top-left
			}
		}
		
	

		

	}

	PlaneGeometry::PlaneGeometry() { //default size 1 , subdivisions 1


		GeneratePlane(1, 1, 1, 1);

	}
	PlaneGeometry::PlaneGeometry(float width, float height) {

		GeneratePlane(width,height, 1, 1);
	}
	PlaneGeometry::PlaneGeometry(float width, float height, int widthSubdivision, int heightSubdivision) {

		GeneratePlane(width, height, widthSubdivision, heightSubdivision);

	}

	PlaneGeometry* PlaneGeometry::clone() {

		return new PlaneGeometry(*this);
	}

	GeometryType PlaneGeometry::GetGeometryType() {
		return GeometryType::PlaneGeometry;
	}
	

	//BOXGEOMETRY

	BoxGeometry::BoxGeometry(){
	
	
		this->width = 1;
		this->height = 1;
		this->depth = 1;
		this->widthSubdivision = 1;
		this->heightSubdivision = 1;
		this->depthSubdivision = 1;

		this->ReservePositionAndIndices(this->widthSubdivision, this->heightSubdivision, this->depthSubdivision);
		this->CreateBox();
		
	
	}
	BoxGeometry::BoxGeometry(float width_, float height_, float depth_){

		this->width = width_;
		this->height = height_;
		this->depth = depth_;
		this->widthSubdivision = 1;
		this->heightSubdivision = 1;
		this->depthSubdivision = 1;

		this->ReservePositionAndIndices(this->widthSubdivision, this->heightSubdivision, this->depthSubdivision);
		this->CreateBox();
	
	}
	BoxGeometry::BoxGeometry(float width_, float height_, float depth_,
		int widthSubdivision_, int heightSubdivision_, int depthSubDivision_) {
	
		this->width = width_;
		this->height = height_;
		this->depth = depth_;
		this->widthSubdivision = widthSubdivision_;
		this->heightSubdivision = heightSubdivision_;
		this->depthSubdivision = depthSubDivision_;

		this->ReservePositionAndIndices(this->widthSubdivision, this->heightSubdivision, this->depthSubdivision);
		this->CreateBox();
	}

	BoxGeometry::BoxGeometry(const BoxGeometry& other) {

		
		if( this != &other ) {
			Geometry::operator=(other);
			this->width = other.width;
			this->height = other.height;
			this->depth = other.depth;
			this->widthSubdivision = other.widthSubdivision;
			this->heightSubdivision = other.heightSubdivision;
			this->depthSubdivision = other.depthSubdivision;
		}

	}


	BoxGeometry& BoxGeometry::operator=(const BoxGeometry& other) {

		if (this != &other) {
			Geometry::operator=(other);
			this->width = other.width;
			this->height = other.height;
			this->depth = other.depth;
			this->widthSubdivision = other.widthSubdivision;
			this->heightSubdivision = other.heightSubdivision;
			this->depthSubdivision = other.depthSubdivision;
		}

		return *this;
	}

	void BoxGeometry::ReservePositionAndIndices(int widthSubdivision, int heightSubdivision, int depthSubDivision) {

		const int vertsPerFace = (widthSubdivision + 1) * (heightSubdivision + 1);
		const int vertsPerEdge = (widthSubdivision + 1);
		const int vertsPerLayer = (heightSubdivision + 1);

		const int faceVertexBudget = vertsPerFace;               // per face
		const int totalVertexBudget = 6 * faceVertexBudget;       // 6 faces

		const int trisPerFace = widthSubdivision * heightSubdivision * 2;
		const int totalIndexBudget = 6 * trisPerFace * 3;        // 3 indices / tri

		position.values.reserve(totalVertexBudget * 1.05f);  // tiny slack
		indices.reserve(totalIndexBudget * 1.05f);

	}


	void BoxGeometry::CreateBox() {

		 this->topFace();
		 this->bottomFace();
		 this->leftFace();
		 this->rightFace();
		 this->frontFace();
		 this->backFace();

	}


	void BoxGeometry::topFace(){

		MyVectorV3::Vector3f startPos(-width / 2, height / 2, -depth / 2);
		MyVectorV3::Vector3f xAxis(1, 0, 0);
		MyVectorV3::Vector3f yAxis(0, 0, 1);
		
		this->createFace(startPos, xAxis, yAxis);
		}
	void BoxGeometry::bottomFace() {

		MyVectorV3::Vector3f startPos(-width / 2, -height / 2, depth / 2);
		MyVectorV3::Vector3f xAxis(1, 0, 0);
		MyVectorV3::Vector3f yAxis(0, 0, -1);

		this->createFace(startPos, xAxis, yAxis);
	}
	void BoxGeometry::leftFace(){
		MyVectorV3::Vector3f startPos(-width / 2, -height / 2, depth / 2);
		MyVectorV3::Vector3f xAxis(0, 0, -1);
		MyVectorV3::Vector3f yAxis(0, 1, 0);
		
		this->createFace(startPos, xAxis, yAxis); 

		}
	void BoxGeometry::rightFace(){

		MyVectorV3::Vector3f startPos(width / 2, -height / 2, -depth / 2);
		MyVectorV3::Vector3f xAxis(0, 0, 1);
		MyVectorV3::Vector3f yAxis(0, 1, 0);

		this->createFace(startPos, xAxis, yAxis);

		}
	void BoxGeometry::frontFace(){

		MyVectorV3::Vector3f startPos(-width / 2, -height / 2, -depth / 2);
		MyVectorV3::Vector3f xAxis(1, 0, 0);
		MyVectorV3::Vector3f yAxis(0, 1, 0);

		this->createFace(startPos, xAxis, yAxis);
		}
	void BoxGeometry::backFace(){

		MyVectorV3::Vector3f startPos(width / 2, -height / 2, depth / 2);
		MyVectorV3::Vector3f xAxis(-1, 0, 0);
		MyVectorV3::Vector3f yAxis(0, 1, 0);

		this->createFace(startPos, xAxis, yAxis);

		}


	void BoxGeometry::createFace(MyVectorV3::Vector3f startPos, MyVectorV3::Vector3f xAxis,
		MyVectorV3::Vector3f yAxis) {


		float segmentWidth = width / widthSubdivision;
		float segmentHeight = height / heightSubdivision;
		float segmentDepth = depth / depthSubdivision;

		MyVectorV3::Vector3f direction(segmentWidth, segmentHeight, segmentDepth);
		MyVectorV3::Vector3f count(widthSubdivision, heightSubdivision, depthSubdivision);

		MyVectorV3::Vector3f  directionX = xAxis * direction;
		float countX = (xAxis * count).Length();

		MyVectorV3::Vector3f  directionY = yAxis * direction; 
		float countY = (yAxis * count).Length();

		float indexStart = this->position.values.size() / 3;  

		for (int y = 0; y <= countY; y++) {

			MyVectorV3::Vector3f newPos = startPos + directionY * y;
			for (int x = 0; x <= countX; x++) {
				MyVectorV3::Vector3f newPos2=newPos + directionX * x;
				this->position.values.push_back(newPos2.x);
				this->position.values.push_back(newPos2.y);
				this->position.values.push_back(newPos2.z);

				this->texcoord.values.push_back((float)x / countX);
				this->texcoord.values.push_back((float)y / countY);

				newPos2.Normalize();
				this->normal.values.push_back(newPos2.x);
				this->normal.values.push_back(newPos2.y);
				this->normal.values.push_back(newPos2.z);
			}

		}
		

		for (int i = 0; i < countY; i++) {
			for (int j = 0; j < countX; j++) {
				int a = indexStart + (i * (countX + 1) + j);
				int b = indexStart + (i * (countX + 1) + j + 1);
				int c = indexStart + ((i + 1) * (countX + 1) + j);
				int d = indexStart + ((i + 1) * (countX + 1) + j + 1);
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);
				indices.push_back(b);
				indices.push_back(d);
				indices.push_back(c);
			}
		}

	}


	BoxGeometry* BoxGeometry::clone() {

		return new BoxGeometry(*this);
	}

	GeometryType BoxGeometry::GetGeometryType() {
		return GeometryType::BoxGeometry;
	}

	//SPHERE GEOMETRY

	SphereGeometry::SphereGeometry() {
		this->radius = 0.5;
		this->widthSubdivision = 16;
		this->heightSubdivision = 16;

		CreateSphere();
	}

	SphereGeometry::SphereGeometry(const SphereGeometry& other) {
		if(this != &other ) {
			Geometry::operator=(other);
			this->radius = other.radius;
			this->widthSubdivision = other.widthSubdivision;
			this->heightSubdivision = other.heightSubdivision;
		}

	}

	SphereGeometry& SphereGeometry::operator=(const SphereGeometry& other) {

		if( this != &other ) {
			Geometry::operator=(other);
			this->radius = other.radius;
			this->widthSubdivision = other.widthSubdivision;
			this->heightSubdivision = other.heightSubdivision;
		}

		return *this;
	}

	SphereGeometry::SphereGeometry(float radius_) {
		this->radius = radius_;
		this->widthSubdivision = 16;
		this->heightSubdivision = 16;


		CreateSphere();
	}

	SphereGeometry::SphereGeometry(float radius_, int widthSubdivision_, int heightSubdivision_) {
		this->radius = radius_;
		this->widthSubdivision = widthSubdivision_;
		this->heightSubdivision = heightSubdivision_;


		CreateSphere();
	}

	void SphereGeometry::CreateSphere() {

		MyVectorV3::Vector3f up(0, radius, 0);
		MyVectorV3::Vector3f normalUp(0, 1, 0);
		MyVectorV3::Vector2f texcoordUp(0.5, 1);

		MyVectorV3::Vector3f down(0, -radius, 0);
		MyVectorV3::Vector3f normalDown(0, -1, 0);
		MyVectorV3::Vector2f texcoordDown(0.5, 0);


		float thetaStep = 2 * M_PI / widthSubdivision;
		float phiStep = M_PI / heightSubdivision;

		float thetaStart = 0;
		float phiStart = M_PI / 2;

		float vStart = 1;
		float vEnd = 0;

		function<MyVectorV3::Vector3f(float,float)> RadToCoord= [this](float theta, float phi) {
			float x = radius * cos(theta) * cos(phi);
			float y = radius * sin(phi);
			float z = radius * cos(phi) * sin(theta);
			return MyVectorV3::Vector3f(x, y, z);
			};



		//top pole
		this->addVertex(up, texcoordUp, normalUp);


		//inner vertices
		for (int i = 1; i < heightSubdivision ; i++) {

			float phi = phiStart - i * phiStep;
			float v = vStart - (float)i / heightSubdivision;

			for (int j = 0; j <= widthSubdivision; ++j) {
				float theta = thetaStart + j * thetaStep;   
				MyVectorV3::Vector3f newPos = RadToCoord(theta, phi);

				MyVectorV3::Vector2f uv;
				uv.x = static_cast<float>(j) / widthSubdivision;   // 0 … 1
				uv.y = v;

				MyVectorV3::Vector3f normal = newPos;
				normal.Normalize();

				this->addVertex(newPos, uv, normal);
			}


		}

		//bottom pole
		this->addVertex(down, texcoordDown, normalDown);


		//indices
		addIndices();

	}


	void SphereGeometry::addVertex(MyVectorV3::Vector3f pos, MyVectorV3::Vector2f uv,
		MyVectorV3::Vector3f normal) {

		this->position.values.push_back(pos.x);
		this->position.values.push_back(pos.y);
		this->position.values.push_back(pos.z);

		this->texcoord.values.push_back(uv.x);
		this->texcoord.values.push_back(uv.y);

	
		this->normal.values.push_back(normal.x);
		this->normal.values.push_back(normal.y);
		this->normal.values.push_back(normal.z);


	}

	void SphereGeometry::addIndices() {

		function<int(int, int)> getIndex = [this](int height,int width) {
			return 1 + height * (this->widthSubdivision+1) + width;
			};

		//top pole
		for (int j = 0; j < widthSubdivision; ++j) {
			indices.push_back(0);                  // pole
			indices.push_back(j + 1);              // current sector on first ring
			indices.push_back(j + 2);   // next sector (wrap)
		}



		//middle quads

		for (int i = 0; i < heightSubdivision - 2; i++) {
			for (int j = 0; j < this->widthSubdivision; j++) {
				int a = getIndex(i, j);
				int b = getIndex(i + 1, j);
				int c = getIndex(i + 1, j + 1);
				int d = getIndex(i, j + 1);
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);
				indices.push_back(a);
				indices.push_back(c);
				indices.push_back(d);
			}
		}


		//bottom pole
		int bottomPoleIndex = this->position.values.size() / 3 - 1;
		for (int i = 0; i < this->widthSubdivision; i++) {
			indices.push_back(bottomPoleIndex);
			indices.push_back(getIndex(heightSubdivision - 2, i));
			indices.push_back(getIndex(heightSubdivision - 2, i + 1));
		}


	}


	SphereGeometry* SphereGeometry::clone() {

		return new SphereGeometry(*this);
	}

	GeometryType SphereGeometry::GetGeometryType() {
		return GeometryType::SphereGeometry;
	}



	LineGeometry::LineGeometry(): Geometry() {



	}

	LineGeometry::LineGeometry(const LineGeometry& other) {

		if( this != &other ) {
			Geometry::operator=(other);
		}
	}
	LineGeometry& LineGeometry::operator=(const LineGeometry& other) {

		if( this != &other ) {
			Geometry::operator=(other);
		}

		return *this;
	}

	void LineGeometry::addPoint(MyVectorV3::Vector3f pos) {

		this->position.values.push_back(pos.x);
		this->position.values.push_back(pos.y);
		this->position.values.push_back(pos.z);
	}

	void LineGeometry::updatePoint(int index, MyVectorV3::Vector3f pos) {

		int realIndex = index * 3;

		this->position.values[realIndex] = pos.x;
		this->position.values[realIndex + 1] = pos.y;
		this->position.values[realIndex + 2] = pos.z;

	}

	void LineGeometry::erasePoint(int index, int count) {

		if (index < 0 || count <= 0) return;
		if (index >= this->position.values.size()/3) return;


		auto itStart = this->position.values.begin() + index * 3;
		auto itEnd = itStart + count * 3;
		this->position.values.erase(itStart, itEnd);

	}
	MyVectorV3::Vector3f LineGeometry::getPoint(int index) {

		int realIndex = index * 3;

		return MyVectorV3::Vector3f(this->position.values[realIndex],
			this->position.values[realIndex + 1],
			this->position.values[realIndex + 2]);
	}

	int LineGeometry::getPointCount() {

		return this->position.values.size() / 3;	
	}

	GeometryType LineGeometry::GetGeometryType() {

		return GeometryType::LineGeometry;

	}

	//MATERIAL

	Material::Material(const Material& other) {

		if(this!= &other ) {
			
			this->vertexShaderText = this->shader.getVertexShaderText();
			this->fragmentShaderText = this->shader.getFragmentShaderText();
			this->geometryShaderText = this->shader.getGeometryShaderText();
			if (other.vertexShaderText != nullptr) {
				*this->vertexShaderText = *other.vertexShaderText;
			}
			if (other.fragmentShaderText != nullptr) {
				*this->fragmentShaderText = *other.fragmentShaderText;

			}
			if (other.geometryShaderText != nullptr) {
			*this->geometryShaderText = *other.geometryShaderText;
			}
			
			this->uniforms = other.uniforms;
			this->isCompiled_ = false;
			this->transparent = other.transparent;
			
			
		}
		

	}

	Material& Material::operator=(const Material& other) {
		if (this != &other) {
			
			this->vertexShaderText = this->shader.getVertexShaderText();
			this->fragmentShaderText = this->shader.getFragmentShaderText();
			this->geometryShaderText = this->shader.getGeometryShaderText();
			if (other.vertexShaderText != nullptr) {
				*this->vertexShaderText = *other.vertexShaderText;
			}
			if (other.fragmentShaderText != nullptr) {
				*this->fragmentShaderText = *other.fragmentShaderText;

			}
			if (other.geometryShaderText != nullptr) {
				*this->geometryShaderText = *other.geometryShaderText;
			}
			this->uniforms = other.uniforms;
			this->isCompiled_ = false;
			this->transparent = other.transparent;
			
		}
		return *this;

	}

	void Material::eraseIfUniformAlready(string uniName) {


		auto it = uniforms.begin();

		bool erase = false;
		for (; it != uniforms.end(); it++) {

			std::visit([&erase, &uniName](auto v) {

				if (uniName == v.uniformName) {
					erase = true;

				}

				}, *it);


			if (erase) {
				uniforms.erase(it);
				erase = false;
				break;
			}

		}


	}


	void Material::setUniformLocations() {

		unsigned int shaderAddress = this->shader.shader->getShaderAddress();
		
		for_each(this->uniforms.begin(), this->uniforms.end(), [&shaderAddress,this](MaterialUniform::UniformVariant& uni) {

			std::visit([&shaderAddress,this](auto& uni_) {


				uni_.uniformLocation = glGetUniformLocation(shaderAddress, uni_.uniformName.c_str());
				if (uni_.uniformLocation == -1 && isDebugModeUniformsAddress) {
					//cout << "Warning! Unable to get the location of uniform " << uni_.uniformName << endl;
					return -1;
				}

				}, uni);

			});


	}


	

	void Material::setWireframe(bool isWireframe) {

		if (isWireframe) {

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		}

	}

	void Material::Compile(Geometry* geometry) {

		this->isCompiled_ = true;

		
		CreateShader();
		BindShader();
		setUniformLocations();
		glUseProgram(0);

	}

	void Material::CreateShader() {

		//after editing shader text by replaceshaderhooks finally for creating program
		if (this->isDebugShaderText) {

			cout << *this->vertexShaderText << endl << endl
				<< "-----------------------" << endl << endl
				<< *this->fragmentShaderText << endl;


			ofstream off;
			off.open(ENGINE_MATERIAL_SHADERS_DIR "/Debug/debugShaders.txt", std::ios::app);
			off << *this->vertexShaderText
				<< endl << endl
				<< "--------------------"
				<< endl << endl
				<< *this->fragmentShaderText
				<< endl << endl
				<< "--------------------"
				<< endl << endl;

			off.close();

		}

		this->shader.shader->createProgram();

	}

	void Material::BindShader() {

		this->shader.bindShader();

	}

	void Material::setModelMatrix(MyMatrixV3::Matrix4f modelMatrix) {

		this->setUniform("modelMatrix", modelMatrix);

	}


	//RAW SHADER MATERIAL----------------------------------------------------------------

	RawShaderMaterial::RawShaderMaterial(const RawShaderMaterial& other) :Material(other) {
		
	}

	RawShaderMaterial& RawShaderMaterial::operator=(const RawShaderMaterial& other) {

		if (this != &other) {
			Material::operator=(other);
		}
		return *this;
	}

	string RawShaderMaterial::LoadShaderFile(string fileLocation) {

		ifstream file(fileLocation);
		if (!file.is_open()) {
			cout << "Error! Unable to open file: " << fileLocation << endl;
			return "";
		}
		
		ostringstream oss;
		string line;https://www.youtube.com/

		while (std::getline(file, line)){
			oss << line << endl;
		}

		return oss.str();
		
	}
	string RawShaderMaterial::LoadVertexShaderFromFile(string vertexShaderFileLocation) {

		return RawShaderMaterial::LoadShaderFile(vertexShaderFileLocation);

	 }
	string RawShaderMaterial::LoadFragmentShaderFromFile(string fragmentShaderFileLocation) {

		return RawShaderMaterial::LoadShaderFile(fragmentShaderFileLocation);

	 }
	
	
	RawShaderMaterial::RawShaderMaterial(string vertexShader, string fragmentShader):Material() {

		vertexShaderText = this->shader.getVertexShaderText();
		fragmentShaderText = this->shader.getFragmentShaderText();

		*vertexShaderText = vertexShader;
		*fragmentShaderText = fragmentShader;
	}
	void RawShaderMaterial::setVertexShader(string vertexShader) {

		vertexShaderText = this->shader.getVertexShaderText();
		*vertexShaderText = vertexShader;

	}
	void RawShaderMaterial::setFragmentShader(string fragmentShader) {

		fragmentShaderText = this->shader.getFragmentShaderText();
		*fragmentShaderText = fragmentShader;

	}

	void RawShaderMaterial::setGeometryShader(string geometryShader) {

		geometryShaderText = this->shader.getGeometryShaderText();
		*geometryShaderText = geometryShader;

	}

	string* RawShaderMaterial::getVertexShader() {

		return this->vertexShaderText;

	}
	string* RawShaderMaterial::getFragmentShader() {


		return this->fragmentShaderText;

	}

	string* RawShaderMaterial::getGeometryShader() {


		return this->geometryShaderText;

	}


	void RawShaderMaterial::setViewProjectionMatrix(MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix) {

		if (vertexShaderText->find("viewProjectionMatrix") != string::npos) {
			MyMatrixV3::Matrix4f viewProjMatrix = projMatrix * viewMatrix;
			this->setUniform("viewProjectionMatrix", viewProjMatrix);
		}
		
		if (vertexShaderText->find("viewMatrix") != string::npos) {
			this->setUniform("viewMatrix", viewMatrix);
		}

		if (vertexShaderText->find("projectionMatrix") != string::npos) {
			this->setUniform("projectionMatrix", projMatrix);
		}


	}
	int RawShaderMaterial::getLayoutNumber(string line) {

		int start = line.find("(");
		int end = line.find(")");

		string search = line.substr(start, end - start + 1);
		string::size_type startNumber = search.find_first_of("0123456789");
		string::size_type endNumber = search.find_last_of("0123456789");

		istringstream iss;
		iss.str(search.substr(startNumber, endNumber - startNumber + 1));

		int ret;
		iss >> ret;

		return ret;
	}


	void RawShaderMaterial::setAttributeLayout(string line, int layout, Geometry* geometry) {


		if (line.find("position") != string::npos) {
			geometry->position.layout_ = layout;
		}
		else if (line.find("uv") != string::npos) {
			geometry->texcoord.layout_ = layout;
		}
		else if (line.find("normal") != string::npos) {
			geometry->normal.layout_ = layout;
		}
		else {

			auto it = geometry->bufferAttributes.begin();
			for (; it != geometry->bufferAttributes.end(); it++) {
				if (line.find(it->shaderName) != string::npos) {
					it->layout_ = layout;
					break;
				}
			}

			auto it2 = geometry->instancedBufferAttributes.begin();
			for (; it2 != geometry->instancedBufferAttributes.end(); it2++) {
				if (line.find(it2->shaderName) != string::npos) {
					it2->layout_ = layout;
					break;
				}
			}

		}
		
	}


	void RawShaderMaterial::updateBufferGeometryLayout(Geometry* geometry){

		istringstream iss;
		iss.str(*vertexShaderText);

		string line;
		while (getline(iss, line, '\n')) {

			if (line.find("layout") != string::npos) {
				int layoutNumber = getLayoutNumber(line);
				setAttributeLayout(line, layoutNumber, geometry);
			}

		}

	}


	RawShaderMaterial* RawShaderMaterial::clone() {

		return new RawShaderMaterial(*this);

	}

	void RawShaderMaterial::Compile(Geometry* geometry) {

		this->updateBufferGeometryLayout(geometry);
		Material::Compile(geometry);
		

	}


	//LINE MATERÝAL --------------------------------------------------------

	LineMaterial::LineMaterial() {

		this->setUniform("lineColor", MyVectorV3::Vector3f(1, 1, 1));

		vertexShaderText = this->shader.getVertexShaderText();
		fragmentShaderText = this->shader.getFragmentShaderText();

		*vertexShaderText = R"(
			
				#version 430 core
				
				layout (location = 0) in vec3 position;
				
				uniform mat4 modelMatrix;
				uniform mat4 view;
				uniform mat4 projection;
				
				void main()
				{
					vec4 worldPosition = modelMatrix * vec4(position, 1.0);
				    gl_Position = projection * view * vec4(position, 1.0);
				}

		)";
		*fragmentShaderText = R"(
				
				#version 430 core
				
				out vec4 FragColor;
				
				uniform vec3 lineColor;
				
				void main()
				{
				    FragColor = vec4(lineColor, 1.0);
				}

		)";
	}

	LineMaterial::LineMaterial(const LineMaterial& cp) : Material(cp) {



	}

	LineMaterial& LineMaterial::operator=(const LineMaterial& cp) {

		if (&cp != this) {
			Material::operator=(cp);
		}

		return *this;

	}

	int LineMaterial::getLayoutNumber(string line) {

		int start = line.find("(");
		int end = line.find(")");

		string search = line.substr(start, end - start + 1);
		string::size_type startNumber = search.find_first_of("0123456789");
		string::size_type endNumber = search.find_last_of("0123456789");

		istringstream iss;
		iss.str(search.substr(startNumber, endNumber - startNumber + 1));

		int ret;
		iss >> ret;

		return ret;
	}


	void LineMaterial::setAttributeLayout(string line, int layout, Geometry* geometry) {


		if (line.find("position") != string::npos) {
			geometry->position.layout_ = layout;
		}
		else if (line.find("uv") != string::npos) {
			geometry->texcoord.layout_ = layout;
		}
		else if (line.find("normal") != string::npos) {
			geometry->normal.layout_ = layout;
		}
		else {

			auto it = geometry->bufferAttributes.begin();
			for (; it != geometry->bufferAttributes.end(); it++) {
				if (line.find(it->shaderName) != string::npos) {
					it->layout_ = layout;
					break;
				}
			}

			auto it2 = geometry->instancedBufferAttributes.begin();
			for (; it2 != geometry->instancedBufferAttributes.end(); it2++) {
				if (line.find(it2->shaderName) != string::npos) {
					it2->layout_ = layout;
					break;
				}
			}

		}

	}


	void LineMaterial::updateBufferGeometryLayout(Geometry* geometry) {

		istringstream iss;
		iss.str(*vertexShaderText);

		string line;
		while (getline(iss, line, '\n')) {

			if (line.find("layout") != string::npos) {
				int layoutNumber = getLayoutNumber(line);
				setAttributeLayout(line, layoutNumber, geometry);
			}

		}

	}

	void LineMaterial::setColor(float r, float g, float b){
		this->setUniform("lineColor", MyVectorV3::Vector3f(r, g, b));
	}
	void LineMaterial::setColor(MyVectorV3::Vector3f color) {
		this->setUniform("lineColor", color);

	}


	void LineMaterial::setViewProjectionMatrix(MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix) {

		this->setUniform("view", viewMatrix);
		this->setUniform("projection", projMatrix);

	}

	void LineMaterial::Compile(Geometry* geometry) {
		this->updateBufferGeometryLayout(geometry);
		Material::Compile(geometry);
	}


	LineMaterial* LineMaterial::clone() {

		return new LineMaterial(*this);
	}

	//MESH BASÝC CMATERIAL----------------------------------------------------------------



	MeshBasicMaterial::MeshBasicMaterial(): Material() {

		//vsText, fsText sadece string olarak atanýr
		this->shader.setShaderTextOnly(ENGINE_MATERIAL_SHADERS_DIR "/material_vs.vs",
			ENGINE_MATERIAL_SHADERS_DIR "/material_fs.fs");


		this->vertexShaderText = this->shader.getVertexShaderText();
		this->fragmentShaderText = this->shader.getFragmentShaderText();

	}

	MeshBasicMaterial::MeshBasicMaterial(const MeshBasicMaterial& other): Material(other) {

		this->extraFragmentHooks = other.extraFragmentHooks;
		this->extraVertexHooks = other.extraVertexHooks;
		
		this->isColor = other.isColor;
		this->isMap = other.isMap;

		this->color = other.color;
		this->map = other.map;
		
		

	}

	MeshBasicMaterial& MeshBasicMaterial::operator=(const MeshBasicMaterial& other) {

		if (this != &other) {
			this->extraFragmentHooks = other.extraFragmentHooks;
			this->extraVertexHooks = other.extraVertexHooks;


			this->isColor = other.isColor;
			this->isMap = other.isMap;

			this->color = other.color;
			this->map = other.map;
			
			Material::operator=(other);
		}

		return *this;

	}


	void MeshBasicMaterial::eraseRemainingHooks() {

		void(*eraseHook)(string * shaderText) = [](string* shaderText) {

			istringstream iss;
			iss.str(*shaderText);
			ostringstream oss;

			while (!iss.eof()) {
				
				string line;
				getline(iss, line, '\n');

				string::size_type findbegin = line.find("<");
				string::size_type findend = line.find(">");
				string::size_type findCheck = line.find_first_of("&|()");

				if ((findbegin != string::npos) &&
					(findend != string::npos) &&
					(findCheck == string::npos)) {

					continue;
				}

				oss << line << endl;

			}

			*shaderText = oss.str();

		};


		eraseHook(vertexShaderText);
		eraseHook(fragmentShaderText);

	}

	void MeshBasicMaterial::replaceShaderHooks(Geometry* geometry) {


		attributeHook.addAttributeShaderCode(geometry->position.getAttributeNameForHook());
		attributeHook.addAttributeShaderCode(geometry->texcoord.getAttributeNameForHook());
		attributeHook.addAttributeShaderCode(geometry->normal.getAttributeNameForHook());
		attributeHook.addAttributeShaderCode(geometry->tangent.getAttributeNameForHook());

		list<BufferAttribute> extraAttributes = geometry->getExtraBufferAttributes();
		for_each(extraAttributes.begin(), extraAttributes.end(), [this](BufferAttribute& buff) {
			attributeHook.addAttributeShaderCode(buff.getAttributeNameForHook());
			});


		list<InstancedAttribute> instancedAttributes = geometry->getInstancedBufferAttributes();
		for_each(instancedAttributes.begin(), instancedAttributes.end(), [this](InstancedAttribute& buff) {
			attributeHook.addAttributeShaderCode(buff.getAttributeNameForHook());
			});



		attributeHook.Replace(vertexShaderText);
		commonHook.Replace(vertexShaderText);
		commonHook.Replace(fragmentShaderText);
		beginVertexHook.Replace(vertexShaderText);
		beginFragmentHook.Replace(fragmentShaderText);
		beginFragmentResultHook.Replace(fragmentShaderText);
		beginNormalFragmentHook.Replace(fragmentShaderText);
		beginNormalFragmentDoubleSideHook.Replace(fragmentShaderText);
		endFragmentGammaEnabledHook.Replace(fragmentShaderText);
		outFragmentColorHook.Replace(fragmentShaderText);
		endFragmentResultHook.Replace(fragmentShaderText);
		

		//extra hooks
		for_each(extraVertexHooks.begin(), extraVertexHooks.end(), [this](MaterialShaderHooks::Hook* hook) {
			hook->Replace(vertexShaderText);
			});

		for_each(extraFragmentHooks.begin(), extraFragmentHooks.end(), [this](MaterialShaderHooks::Hook* hook) {
			hook->Replace(fragmentShaderText);
			});
	}
	

	void MeshBasicMaterial::setColor(float r, float g, float b, float w) {

	
		setUniform<MyVectorV3::Vector4f>("uColor", MyVectorV3::Vector4f(r, g, b, w));
		
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::ColorVertexHook());
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::BeginColorVertexHook());
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::EndColorVertexHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::ColorFragmentHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::FragmentColorResultHook());

		
		//extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::BeginFragmentManualGammaHook());
		
		this->color = MyVectorV3::Vector4f(r, g, b, w);
		this->isColor = true;
	}
	void MeshBasicMaterial::setMap(MyTextureV2::Texture texture) {

		setUniform("gSampler", texture);
		this->map = texture;
		this->isMap = true;

		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::TextureFragmentHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::FragmentTextureResultHook());
	
		if (texture.internalFormat != GL_SRGB && texture.internalFormat != GL_SRGB_ALPHA
			&& texture.internalFormat != GL_SRGB8 && texture.internalFormat != GL_SRGB8_ALPHA8) {
			//if texture is srgb, then we need to convert it to linear space
			extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::BeginFragmentManualGammaHook());
		}

	}

	void MeshBasicMaterial::setMapAndColor(MyTextureV2::Texture texture, MyVectorV3::Vector4f color, float ratio) {

		setUniform("gSampler", texture);
		setUniform<MyVectorV3::Vector4f>("uColor", color);
		setUniform<float>("uColorToTexture;", ratio);

		this->map = texture;
		this->color = color;
		this->isColor = true;
		this->isMap = true;
		
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::ColorVertexHook());
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::BeginColorVertexHook());
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::EndColorVertexHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::ColorFragmentHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::TextureFragmentHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::ColorAndTextureFragmentHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::FragmentColorAndTextureResultHook());


		if (texture.internalFormat != GL_SRGB && texture.internalFormat != GL_SRGB_ALPHA
			&& texture.internalFormat != GL_SRGB8 && texture.internalFormat != GL_SRGB8_ALPHA8) {
			
			extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::BeginFragmentManualGammaHook());
		}

	}

	MyTextureV2::Texture MeshBasicMaterial::getMapTexture() {

		if (this->isMap) {

			return this->map;
		}
		else {

			cout << "MESH MATERIAL HAS NO MAP, NULL TEXTURE HAS BEEN SENT" << endl;
			return MyTextureV2::Texture(-1);
		}
	}
	MyVectorV3::Vector4f MeshBasicMaterial::getColor() {
		if (this->isColor) {

			return this->color;
		}
		else {

			cout << "MESH MATERIAL HAS NO COLOR, NULL COLOR HAS BEEN SENT" << endl;
			return MyVectorV3::Vector4f(0, 0, 0, 0);
		}

	}

	bool MeshBasicMaterial::IsMap() {

		return this->isMap;

	}
	bool MeshBasicMaterial::IsColor() {


		return this->isColor;
	}

	void MeshBasicMaterial::enableSkinning() {
	
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::AttributesBone());
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::SkinningVertexMain());

	}

	void MeshBasicMaterial::enableAssimpModelHooks() {

		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::AssimpTransparency());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::EndFragmentResultAssimp());

	}

	void MeshBasicMaterial::setViewProjectionMatrix(MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix) {

		MyMatrixV3::Matrix4f viewProjMatrix = projMatrix * viewMatrix;
		this->setUniform("viewProjectionMatrix", viewProjMatrix);
		this->setUniform("viewMatrix", viewMatrix);
		this->setUniform("projectionMatrix", projMatrix);

	}

	void MeshBasicMaterial::Compile(Geometry* geometry) {

		

		this->isCompiled_ = true;

		replaceShaderHooks(geometry);
		eraseRemainingHooks();
		
		CreateShader();
		BindShader();
		setUniformLocations();
		glUseProgram(0);

	}

	void MeshBasicMaterial::onBeforeCompile(function<void(string* vs, string* fs)> func) {
		func(this->vertexShaderText, this->fragmentShaderText);
	}

	
	void MeshBasicMaterial::setCameraLocalPos(MyVectorV3::Vector3f camLocalPos){

	
		this->setUniform("gCameraLocalPos", camLocalPos);

		}

	MeshBasicMaterial* MeshBasicMaterial::clone() {

		return new MeshBasicMaterial(*this);

	}
	

	//MESHSTANDARDMATERIAL

	MeshStandardMaterial::MeshStandardMaterial() :MeshBasicMaterial() {




	}

	MeshStandardMaterial::MeshStandardMaterial(const MeshStandardMaterial& other) :MeshBasicMaterial(other) {
		this->lightCommonHook = other.lightCommonHook;
		this->isDirectionalLightCastShadow = other.isDirectionalLightCastShadow;
		this->displacementMap = other.displacementMap;
		this->displacementMapIntensity = other.displacementMapIntensity;
		this->isDisplacementMap = other.isDisplacementMap;
		this->normalMap = other.normalMap;
		this->isNormalMap = other.isNormalMap;
		this->spotLightNumber = other.spotLightNumber;
		this->pointLightNumber = other.pointLightNumber;
		this->isDirectionalLightEnabled = other.isDirectionalLightEnabled;
		this->isPointLightEnabled = other.isPointLightEnabled;
		this->isSpotLightEnabled = other.isSpotLightEnabled;
		this->shadow = other.shadow;
		this->doubleSided = other.doubleSided;
		
	}

	MeshStandardMaterial& MeshStandardMaterial::operator=(const MeshStandardMaterial& other) {

		if (this != &other) {
			this->lightCommonHook = other.lightCommonHook;
			this->isDirectionalLightCastShadow = other.isDirectionalLightCastShadow;
			this->displacementMap = other.displacementMap;
			this->displacementMapIntensity = other.displacementMapIntensity;
			this->isDisplacementMap = other.isDisplacementMap;
			this->normalMap = other.normalMap;
			this->isNormalMap = other.isNormalMap;
			this->spotLightNumber = other.spotLightNumber;
			this->pointLightNumber = other.pointLightNumber;
			this->isDirectionalLightEnabled = other.isDirectionalLightEnabled;
			this->isPointLightEnabled = other.isPointLightEnabled;
			this->isSpotLightEnabled = other.isSpotLightEnabled;
			this->shadow = other.shadow;
			this->doubleSided = other.doubleSided;
			MeshBasicMaterial::operator=(other);
		}
		return *this;
	}

	void MeshStandardMaterial::setNormalMap(MyTextureV2::Texture texture) {

		this->isNormalMap = true;
		this->normalMap = texture;
		this->lightCommonHook.isNormalMap = true;
		setUniform("gNormalMap", texture);

		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::OutNormalMapHook());
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::BeginNormalMapVertexHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::InNormalMapHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::NormalMapHook());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::BeginNormalMapFragmentHook());


	}

	MyTextureV2::Texture MeshStandardMaterial::getNormalMap() {

			return this->normalMap;
		
	}

	bool MeshStandardMaterial::IsNormalMap() {
		return this->isNormalMap;
	}

	void MeshStandardMaterial::setDisplacementMap(MyTextureV2::Texture texture, float intensity) {

		this->isDisplacementMap = true;
		this->displacementMap = texture;
		this->displacementMapIntensity = intensity;
		setUniform("uDisplacement.map", texture);
		setUniform("uDisplacement.intensity", intensity);

		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::DisplacementMapVertexHook());
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::DisplacementMapVertexBeginHook());
		

	}

	void MeshStandardMaterial::setDisplacementMapIntensity(float intensity) {

		setUniform("uDisplacement.intensity", intensity);
		this->displacementMapIntensity = intensity;

	}

	MyTextureV2::Texture MeshStandardMaterial::getDisplacementMap() {
		return this->displacementMap;
	}

	float MeshStandardMaterial::getDisplacementMapIntensity() {
		return this->displacementMapIntensity;
	}

	bool MeshStandardMaterial::IsDisplacementMap() {
		return this->isDisplacementMap;
	}

	void MeshStandardMaterial::setNormalMapSettings(Geometry* geometry) {

		if(geometry->tangent.values.size() > 0 ) {
			return; //already has tangent attribute, so we no need to calculate
		}

		if (geometry->indices.size() == 0) {

			int vertexCount = geometry->position.values.size() / 3;
			geometry->indices.resize(vertexCount);
			int i = 0;
			generate(geometry->indices.begin(), geometry->indices.end(), [&i]() {
				int ret = i;
				i++;
				return ret;
				});

		}


		vector<float> tangents;
		tangents.resize(geometry->position.values.size());

		vector<float> bittangents;
		bittangents.resize(geometry->position.values.size());

		for (int i = 0; i < geometry->indices.size(); i += 3) {
			 

			function<MyVectorV3::Vector3f(int)> getVertexPos = [&geometry](int index) {
				MyVectorV3::Vector3f ret;
				ret.x = geometry->position.values[index * 3];
				ret.y = geometry->position.values[index * 3+1];
				ret.z = geometry->position.values[index * 3+2];

				return ret;
			};

			function<MyVectorV3::Vector2f(int)> getVertexUv = [&geometry](int index) {

				MyVectorV3::Vector2f ret;
				ret.x = geometry->texcoord.values[index * 2];
				ret.y = geometry->texcoord.values[index * 2 + 1];
				

				return ret;
			};

			MyVectorV3::Vector3f pos1 = getVertexPos(geometry->indices[i]);
			MyVectorV3::Vector3f pos2 = getVertexPos(geometry->indices[i+1]);
			MyVectorV3::Vector3f pos3 = getVertexPos(geometry->indices[i+2]);
			

			MyVectorV3::Vector2f uv1 = getVertexUv(geometry->indices[i]);
			MyVectorV3::Vector2f uv2 = getVertexUv(geometry->indices[i + 1]);
			MyVectorV3::Vector2f uv3 = getVertexUv(geometry->indices[i + 2]);

			MyVectorV3::Vector3f edge1 = pos2 - pos1;
			MyVectorV3::Vector3f edge2 = pos3 - pos1;

			MyVectorV3::Vector2f deltaUV1 = uv2 - uv1;
			MyVectorV3::Vector2f deltaUV2 = uv3 - uv1;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);


			MyVectorV3::Vector3f tangent1;
			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


			MyVectorV3::Vector3f bitangent;
			bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

			tangents[geometry->indices[i] * 3] += tangent1.x;
			tangents[geometry->indices[i] * 3 + 1 ] += tangent1.y;
			tangents[geometry->indices[i] * 3 + 2] += tangent1.z;

			tangents[geometry->indices[i+1] * 3] += tangent1.x;
			tangents[geometry->indices[i+1] * 3 + 1] += tangent1.y;
			tangents[geometry->indices[i+1] * 3 + 2] += tangent1.z;

			tangents[geometry->indices[i+2] * 3] += tangent1.x;
			tangents[geometry->indices[i+2] * 3 + 1] += tangent1.y;
			tangents[geometry->indices[i+2] * 3 + 2] += tangent1.z;


			bittangents[geometry->indices[i] * 3] += bitangent.x;
			bittangents[geometry->indices[i] * 3 + 1] += bitangent.y;
			bittangents[geometry->indices[i] * 3 + 2] += bitangent.z;

			bittangents[geometry->indices[i + 1] * 3] += bitangent.x;
			bittangents[geometry->indices[i + 1] * 3 + 1] += bitangent.y;
			bittangents[geometry->indices[i + 1] * 3 + 2] += bitangent.z;

			bittangents[geometry->indices[i + 2] * 3] += bitangent.x;
			bittangents[geometry->indices[i + 2] * 3 + 1] += bitangent.y;
			bittangents[geometry->indices[i + 2] * 3 + 2] += bitangent.z;

		}

		int numVertices = geometry->position.values.size() / 3;
		std::vector<float>& tangents4=geometry->tangent.values;
		tangents4.resize(numVertices * 4); // vec4 per vertex

		for (int v = 0; v < numVertices; v++) {

			MyVectorV3::Vector3f n (geometry->normal.values[v * 3], 
				geometry->normal.values[v * 3+1], 
				geometry->normal.values[v * 3+2]);


			MyVectorV3::Vector3f t = MyVectorV3::Vector3f(tangents[v * 3 + 0],
				tangents[v * 3 + 1],
				tangents[v * 3 + 2]);
			t.Normalize();


			MyVectorV3::Vector3f b = MyVectorV3::Vector3f(bittangents[v * 3 + 0],
				bittangents[v * 3 + 1],
				bittangents[v * 3 + 2]);
			b.Normalize();
			
			// Gram-Schmidt orthogonalization (make tangent orthogonal to normal)
			t = (t - n * n.Dot(t));
			t.Normalize();

			// Compute handedness: compare cross(N,T) with B
			float w = (n.Cross(t)).Dot(b) < 0.0f ? -1.0f : 1.0f;

			tangents4[v * 4 + 0] = t.x;
			tangents4[v * 4 + 1] = t.y;
			tangents4[v * 4 + 2] = t.z;
			tangents4[v * 4 + 3] = w;
		}

		//geometry->setAttribute("tangent", OglObject3D::BufferAttribute(tangents4,4));

		GLuint tangentBuffer;
		glGenBuffers(1, &tangentBuffer);


		glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->tangent.values[0])* geometry->tangent.values.size(), &geometry->tangent.values[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(geometry->tangent.layout_);
		glVertexAttribPointer(geometry->tangent.layout_, geometry->tangent.count, GL_FLOAT, GL_FALSE, 0, 0);

	}

	void MeshStandardMaterial::setViewProjectionMatrix(MyMatrixV3::Matrix4f viewMatrix, MyMatrixV3::Matrix4f projMatrix) {



		MeshBasicMaterial::setViewProjectionMatrix(viewMatrix, projMatrix);
	}



	void MeshStandardMaterial::Compile(Geometry* geometry) {


		MeshBasicMaterial::Compile(geometry);

	}


	void MeshStandardMaterial::replaceShaderHooks(Geometry* geometry) {

		MeshBasicMaterial::replaceShaderHooks(geometry);

		this->lightCommonHook.Replace(fragmentShaderText);
		this->lightMainBeginHook.Replace(fragmentShaderText);
		this->lightMainEndHook.Replace(fragmentShaderText);
		lightDirectionalHook.Replace(fragmentShaderText);
		lightPointHook.Replace(fragmentShaderText);
		lightSpotHook.Replace(fragmentShaderText);
		lightMainDirectionalHook.Replace(fragmentShaderText);
		lightMainPointHook.Replace(fragmentShaderText);
		lightMainSpotHook.Replace(fragmentShaderText);
	

		if (shadow.receiveShadow) {


			this->shadowMapVertexHook.Replace(vertexShaderText);
			this->shadowMapVertexBeginHook.Replace(vertexShaderText);
			this->shadowMapFragmentHook.Replace(fragmentShaderText);
		}

		if (shadow.receiveShadow) {

			this->shadowMapDirectionalLightHook.Replace(fragmentShaderText);
		}

		if (shadow.receiveShadow) {

			shadowMapSpotLightHook.Replace(fragmentShaderText);
		}


		if (shadow.receiveShadow) {

			shadowMapPointLightHook.Replace(fragmentShaderText);
		}

		if(this->doubleSided) {
			doubleSideDirectionalFragmentHook.Replace(fragmentShaderText);
			doubleSidePointFragmentHook.Replace(fragmentShaderText);
			doubleSideSpotFragmentHook.Replace(fragmentShaderText);
		}


		void(*modifyShaderText)(string*,string from,string to) = [](string* shaderText,string from, string to) {

			string::size_type find = shaderText->find(from);

			if (find != string::npos) {
				shaderText->replace(find, from.size(), to);
			}
		};

		ostringstream oss;

		string fromDir = "#define MAX_DIR_LIGHT_NUMBER 1";
		oss << "#define MAX_DIR_LIGHT_NUMBER " << dirLightNumber+1;
		modifyShaderText(fragmentShaderText, fromDir, oss.str());


		oss.clear();
		oss.str("");

		string fromPoint = "#define MAX_POINT_LIGHT_NUMBER 1";
		oss << "#define MAX_POINT_LIGHT_NUMBER " << pointLightNumber+1;
		modifyShaderText(fragmentShaderText, fromPoint, oss.str());

		oss.clear();
		oss.str("");

		string fromSpot = "#define MAX_SPOT_LIGHT_NUMBER 1";
		oss << "#define MAX_SPOT_LIGHT_NUMBER " << spotLightNumber+1;
		modifyShaderText(fragmentShaderText, fromSpot, oss.str());

	}

	void MeshStandardMaterial::enableDirectionalLight(int numberDirLight) {

		this->dirLightNumber = numberDirLight;
	}
	void MeshStandardMaterial::enablePointLight(int numberOfPointLight) {

		this->pointLightNumber = numberOfPointLight;
		
	}
	void MeshStandardMaterial::enableSpotLight(int numberOfSpotLight){

		
		this->spotLightNumber = numberOfSpotLight;
	}

	void MeshStandardMaterial::enableEnvironmentMap() {
		this->isEnvironmentMapEnabled = true;

		extraVertexHooks.insert(extraVertexHooks.end(),new MaterialShaderHooks::EnvironmentMapVertex());
		extraVertexHooks.insert(extraVertexHooks.end(), new MaterialShaderHooks::EnvironmentMapVertexBegin());

		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::EnvironmentMapFragment());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::EndFragmentResultEnvironmentMap());
		extraFragmentHooks.insert(extraFragmentHooks.end(), new MaterialShaderHooks::EnvironmentMapNormalMapFragment());

	}

	bool MeshStandardMaterial::getEnvironmentMapEnabled() {

		return isEnvironmentMapEnabled;
	}

	MeshStandardMaterial* MeshStandardMaterial::clone() {

		return new MeshStandardMaterial(*this);

	}


	void MeshStandardMaterialModifier::enableDirectionalLightShadowShader(MeshStandardMaterial* standardMaterial) {

		standardMaterial->isDirectionalLightCastShadow = true;

	}

	bool MeshStandardMaterialModifier::isNormalMap(MeshStandardMaterial* standardMaterial) {

		return standardMaterial->isNormalMap;

	}
	void MeshStandardMaterialModifier::setNormalMapSettings(MeshStandardMaterial* standardMaterial, Geometry* geometry) {

		standardMaterial->setNormalMapSettings(geometry);

	}




	//MESHES

	Mesh::Mesh(){
	
		this->scale = MyVectorV3::Vector3f(1,1,1);
		this->position = MyVectorV3::Vector3f(0, 0, 0);
		this->rotation = MyMatrixV3::Quaternion();
		
	
	}


	Mesh::Mesh(Geometry& geo, Material& material) {

		this->scale = MyVectorV3::Vector3f(1, 1, 1);
		this->position = MyVectorV3::Vector3f(0, 0, 0);
		this->rotation = MyMatrixV3::Quaternion();

		this->geometry = &geo;
		this->material = &material;
		
		
	}

	Mesh::Mesh(Geometry* geo, Material* material) {

		this->scale = MyVectorV3::Vector3f(1, 1, 1);
		this->position = MyVectorV3::Vector3f(0, 0, 0);
		this->rotation = MyMatrixV3::Quaternion();

		this->geometry = geo;
		this->material = material;

	}

	Mesh::Mesh(const Mesh& other) {

		this->geometry = other.geometry;
		this->material = other.material;
		this->scale = other.scale;
		this->position = other.position;
		this->rotation = other.rotation;
		this->VAO = other.VAO;
		this->worldTransform = other.worldTransform;
		this->isLoaded_ = false;
		this->isAssimpAnimatedMesh = other.isAssimpAnimatedMesh;
		this->isAssimpMesh = other.isAssimpMesh;
		this->personalData = other.personalData;
		this->mouseEvents = other.mouseEvents;

	}


	Mesh& Mesh::operator=(const Mesh& other) {
		if (this != &other) {
			this->geometry = other.geometry;
			this->material = other.material;
			this->scale = other.scale;
			this->position = other.position;
			this->rotation = other.rotation;
			this->VAO = other.VAO;
			this->worldTransform = other.worldTransform;
			this->isLoaded_ = false;
			this->isAssimpMesh = other.isAssimpMesh;
			this->isAssimpAnimatedMesh = other.isAssimpAnimatedMesh;
			this->personalData = other.personalData;
			this->mouseEvents = other.mouseEvents;

			
		}
		return *this;
	}


	Mesh* Mesh::clone() {
		RawShaderMaterial* raw = GetMaterial<RawShaderMaterial>();
		
		MeshBasicMaterial* meshbasic = GetMaterial<MeshBasicMaterial>();
		MeshStandardMaterial* meshstandard = GetMaterial<MeshStandardMaterial>();


		BufferGeometry* buffer = GetGeometry<BufferGeometry>();
		PlaneGeometry* plane = GetGeometry<PlaneGeometry>();
		BoxGeometry* box = GetGeometry<BoxGeometry>();
		SphereGeometry* sphere= GetGeometry<SphereGeometry>();

		Mesh* cloneMesh=new Mesh();

		if(raw != nullptr) {
			cloneMesh->material = raw->clone();
		}else if(meshbasic != nullptr) {
			cloneMesh->material = meshbasic->clone();
		}else if (meshstandard != nullptr) {
			cloneMesh->material = meshstandard->clone();
		}


		if (plane != nullptr) {
			cloneMesh->geometry = plane->clone();
		}
		else if (box != nullptr) {
			cloneMesh->geometry = box->clone();
		}
		else if (sphere != nullptr) {
			cloneMesh->geometry = sphere->clone();
		}
		else if(buffer!=nullptr){
			cloneMesh->geometry = buffer->clone();
		}

		return cloneMesh;
	}

	void Mesh::RotateOnAxis(float x, float y, float z, string order) {

		MyMatrixV3::Quaternion q;
		q.SetFromEuler(x, y, z, order);
		this->rotation = this->rotation * q;

	}
	void Mesh::RotateOnAxis(float angle,const MyVectorV3::Vector3f& axis) {

		MyMatrixV3::Quaternion q;
		q.SetFromAxisAngle(angle,axis);
		this->rotation =  this->rotation * q;

	}
	void Mesh::RotateOnAxis(const MyMatrixV3::Matrix4f& rotationMatrix) {

		MyMatrixV3::Quaternion q;
		q.SetFromRotationMatrix(rotationMatrix);
		this->rotation = this->rotation * q;
	}
	void Mesh::RotateOnAxis(const MyMatrixV3::Quaternion& q) {

		this->rotation =  this->rotation * q;
	}

	void Mesh::RotateOnWorldAxis(float x, float y, float z, string order ) {

		MyMatrixV3::Quaternion q;
		q.SetFromEuler(x, y, z, order);
		this->rotation = q * this->rotation;
	}
	void Mesh::RotateOnWorldAxis(float angle, const MyVectorV3::Vector3f& axis) {

		MyMatrixV3::Quaternion q;
		q.SetFromAxisAngle(angle, axis);
		this->rotation = q * this->rotation;

	}
	void Mesh::RotateOnWorldAxis(const MyMatrixV3::Matrix4f& rotationMatrix) {

		MyMatrixV3::Quaternion q;
		q.SetFromRotationMatrix(rotationMatrix);
		this->rotation = q * this->rotation ;
	}
	void Mesh::RotateOnWorldAxis(const MyMatrixV3::Quaternion& q) {

		this->rotation = q * this->rotation ;

	}

	void Mesh::ApplyMatrix(MyMatrixV3::Matrix4f matrix) {
		
		this->matrix.Compose(this->position,rotation, this->scale);

		this->matrix = matrix * this->matrix;
		this->matrix.Decompose(this->position, rotation, this->scale);

		
	}

	MyMatrixV3::Matrix4f Mesh::GetMatrix() {

		this->worldTransform.SetPosition(this->position);
		this->worldTransform.SetRotation(this->rotation);
		this->worldTransform.SetScale(this->scale);

		this->matrix = this->worldTransform.GetMatrix();
		return matrix;

	}

	unsigned int Mesh::getVAO() {

		return this->VAO;

	}

	MyOglWorldTransform::WorldTrans* Mesh::getWorldTransform() {

		this->worldTransform.SetPosition(this->position);
		this->worldTransform.SetRotation(this->rotation);
		this->worldTransform.SetScale(this->scale);

		return &this->worldTransform;
	}

	bool Mesh::isLoaded() {
		return this->isLoaded_;
	}	

	void Mesh::setShadowMapMaterial() {


		string vertexShader = *material->vertexShaderText;
		this->shadowMapMaterial.setVertexShader(vertexShader);;
		


		string fragmentShader= R"(
						#version 430
						layout(location = 0) out vec3 depthTexture;

						void main() {
						
	
						float depth = gl_FragCoord.z;
						depthTexture = vec3(depth, depth, depth);
					})";
		this->shadowMapMaterial.setFragmentShader(fragmentShader);
		this->shadowMapMaterial.Compile(this->geometry);


		this->shadowMapMaterialPointLight.setVertexShader(vertexShader);

		string fragmentShader2 = R"(
						#version 430

						layout(location = 0) out vec3 depthTexture;
						in vec3 FragWorldPos;
						
						uniform vec3 lightPos;
						uniform float far_plane;

				
						void main()
						{

						 // get distance between fragment and light source
						 float lightDistance = length(FragWorldPos.xyz - lightPos);
    
						 // map to [0;1] range by dividing by far_plane
						 lightDistance = lightDistance / far_plane;
    
						 // write this as modified depth
						 gl_FragDepth = lightDistance;
						depthTexture=vec3(lightDistance,lightDistance,lightDistance);
						}  
					)";
		shadowMapMaterialPointLight.setFragmentShader(fragmentShader2);

		string geometryShader = R"(
				#version 430
				layout (triangles) in;
				layout (triangle_strip, max_vertices=18) out;
				
				
				out vec3 FragWorldPos;
				
				uniform mat4 shadowMatrices[6];
			
				
				void main() {
					for(int face = 0; face < 6; face++) {
						gl_Layer = face;
						
						for(int i = 0; i < 3; i++) {

							FragWorldPos = gl_in[i].gl_Position.xyz;
						
							gl_Position = shadowMatrices[face] * gl_in[i].gl_Position;
							EmitVertex();
						}
						EndPrimitive();
					}
				})";
		shadowMapMaterialPointLight.setGeometryShader(geometryShader);
		shadowMapMaterialPointLight.Compile(this->geometry);

	}
	void Mesh::LoadMesh() {

		this->isLoaded_ = true;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		 
		this->personalData["errorMesh"] = "false";

		int posVertexCount = geometry->position.values.size() / 3;
		int uvVertexCount = geometry->texcoord.values.size() / 2;
		int normalVertexCount = geometry->normal.values.size() / 3;

		for (int idx : geometry->indices) {
			if (idx < 0 || idx >= posVertexCount ||
				idx >= uvVertexCount ||
				idx >= normalVertexCount) {

				std::cout << "ERROR: bad index " << idx
					<< " posCount=" << posVertexCount
					<< " uvCount=" << uvVertexCount
					<< " normalCount=" << normalVertexCount
					<< std::endl;
				this->personalData["errorMesh"] = "true";
				return;
			}
		}

		MeshStandardMaterial* standardMaterial = dynamic_cast<MeshStandardMaterial*>(this->material);
		RawShaderMaterial* rawShaderMaterial = dynamic_cast<RawShaderMaterial*>(this->material);

		if (rawShaderMaterial != nullptr) {
			rawShaderMaterial->updateBufferGeometryLayout(this->geometry);
		}

		if (standardMaterial != nullptr) {


			bool isNormalMap = MeshStandardMaterialModifier::isNormalMap(standardMaterial);
			if (isNormalMap) {
				MeshStandardMaterialModifier::setNormalMapSettings(standardMaterial, geometry);	
			}

		}




		int bufferNumber = geometry->getBufferNumber()+1;
		if (geometry->indices.size() == 0) {
			int vertexCount = geometry->position.values.size() / 3;
			for (int i = 0; i < vertexCount; i++) {
				this->geometry->indices.push_back(i);
			}
		}
		
		

		this->m_Buffers = new GLuint[bufferNumber];
		glGenBuffers(bufferNumber, m_Buffers);

		// position
		if (!geometry->position.values.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
			glBufferData(
				GL_ARRAY_BUFFER,
				geometry->position.values.size() * sizeof(float),
				geometry->position.values.data(),
				GL_STATIC_DRAW
			);
			glEnableVertexAttribArray(geometry->position.layout_);
			glVertexAttribPointer(
				geometry->position.layout_,
				geometry->position.count,
				GL_FLOAT,
				GL_FALSE,
				0,
				0
			);
		}
		else {
			std::cout << "ERROR: mesh has no position data\n";
			return;
		}

		if (!geometry->texcoord.values.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[1]);
			glBufferData(
				GL_ARRAY_BUFFER,
				geometry->texcoord.values.size() * sizeof(float),
				geometry->texcoord.values.data(),
				GL_STATIC_DRAW
			);
			glEnableVertexAttribArray(geometry->texcoord.layout_);
			glVertexAttribPointer(
				geometry->texcoord.layout_,
				geometry->texcoord.count,
				GL_FLOAT,
				GL_FALSE,
				0,
				0
			);
		}
		else {
			std::cout << "Warning: mesh has no UVs\n";
		}


		if (!geometry->normal.values.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[2]);
			glBufferData(
				GL_ARRAY_BUFFER,
				geometry->normal.values.size() * sizeof(float),
				geometry->normal.values.data(),
				GL_STATIC_DRAW
			);
			glEnableVertexAttribArray(geometry->normal.layout_);
			glVertexAttribPointer(
				geometry->normal.layout_,
				geometry->normal.count,
				GL_FLOAT,
				GL_FALSE,
				0,
				0
			);
		}
		else {
			std::cout << "Warning: mesh has no normals\n";
		}

		list<OglObject3D::BufferAttribute> extraAttributes = geometry->getExtraBufferAttributes();
		auto it = extraAttributes.begin();
		for (int i = 0; i < extraAttributes.size(); i++) {
			
			if(it->values.size()==0){
				it++;
				continue;
			}

			if (it->value_type == "float") {
				
		
			glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i+3]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(it->values[0]) * it->values.size(), &it->values[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(it->layout_);
			glVertexAttribPointer(it->layout_, it->count, GL_FLOAT, GL_FALSE, 0, 0);
			
			
			}else if(it->value_type=="int"){
				
				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + 3]);
				vector<int> intValues(it->values.begin(), it->values.end());
				glBufferData(GL_ARRAY_BUFFER, sizeof(intValues[0]) * intValues.size(), &intValues[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it->layout_);
				glVertexAttribIPointer(it->layout_, it->count, GL_INT, 0, 0);
			}
			else if (it->value_type == "uint") {
			
				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + 3]);
				vector<unsigned int> uintValues(it->values.begin(), it->values.end());
				glBufferData(GL_ARRAY_BUFFER, sizeof(uintValues[0]) * uintValues.size(), &uintValues[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it->layout_);
				glVertexAttribIPointer(it->layout_, it->count, GL_UNSIGNED_INT, 0, 0);
			}
			
			
			it++;
		}


		int indexOffset = 3 + geometry->getExtraBufferAttributes().size();
		list<OglObject3D::InstancedAttribute> instancedAttributes = geometry->getInstancedBufferAttributes();
		auto it2 = instancedAttributes.begin();
		for (int i = 0; i < instancedAttributes.size(); i++) {

			if (it2->values.size() == 0) {
				it2++;
				continue;
			}

			if (it2->value_type == "float") {


				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + indexOffset]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(it2->values[0]) * it2->values.size(), &it2->values[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it2->layout_);
				glVertexAttribPointer(it2->layout_, it2->count, GL_FLOAT, GL_FALSE, 0, 0);
				glVertexAttribDivisor(it2->layout_, 1);

			}
			else if (it2->value_type == "int") {

				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + indexOffset]);
				vector<int> intValues(it2->values.begin(), it2->values.end());
				glBufferData(GL_ARRAY_BUFFER, sizeof(intValues[0]) * intValues.size(), &intValues[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it2->layout_);
				glVertexAttribIPointer(it2->layout_, it2->count, GL_INT, 0, 0);
				glVertexAttribDivisor(it2->layout_, 1);
			}
			else if (it2->value_type == "uint") {

				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + indexOffset]);
				vector<unsigned int> uintValues(it2->values.begin(), it2->values.end());
				glBufferData(GL_ARRAY_BUFFER, sizeof(uintValues[0]) * uintValues.size(), &uintValues[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it2->layout_);
				glVertexAttribIPointer(it2->layout_, it2->count, GL_UNSIGNED_INT, 0, 0);
				glVertexAttribDivisor(it2->layout_, 1);
			}


			it2++;
		}
	

		list<OglObject3D::SSBOAttribute> extraSSBOAttributes = geometry->getExtraSSBOAttributes();
		auto itSSBO = extraSSBOAttributes.begin();
		for (; itSSBO != extraSSBOAttributes.end(); itSSBO++) {
		
			itSSBO->Use();
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[bufferNumber-1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(geometry->indices[0]) * geometry->indices.size(), &geometry->indices[0], GL_STATIC_DRAW);

		


		glBindVertexArray(0);


	}

	void Mesh::addEventListener(string eventName, function<void()> func) {

		std::transform(eventName.begin(), eventName.end(), eventName.begin(),
			[](unsigned char c) {
				return static_cast<char>(std::tolower(c));
			});

		if (eventName == "mouseenter") {

			this->mouseEvents.mouseEnterEventFunc.push_back(func);
			this->mouseEvents.isMouseEnterEventFuncSet = true;
		}
		else if (eventName == "mouseleave") {
			this->mouseEvents.mouseLeaveEventFunc.push_back(func);
			this->mouseEvents.isMouseLeaveEventFuncSet = true;
		}
		else if (eventName == "click") {
			this->mouseEvents.mouseClickEventFunc.push_back(func);
			this->mouseEvents.isMouseClickEventFuncSet = true;

		}
		else if (eventName == "mousemove") {
			this->mouseEvents.mouseMoveEventFunc.push_back(func);
			this->mouseEvents.isMouseMoveEventFuncSet = true;
		}

	}

	
	void Mesh::Render() {

		//compile if not
		if (!this->material->isCompiled()) {
			material->Compile(geometry);
		}

		//culling for assimp
		if (isAssimpMesh) {
			glEnable(GL_CULL_FACE);
		}
		else {
			glDisable(GL_CULL_FACE);
		}

		//binding shader
		material->BindShader();
		material->setUniformLocations();

		//depth and blend mode
		
		//material->setBlendMode();
		//material->setDepthMode();	

		

		//cout << "depth test: " << material->depthTest << endl;
	//	cout << "depth write: " << material->depthWrite << endl;


		//binding uniforms
		for_each(material->uniforms.begin(), material->uniforms.end(), [](MaterialUniform::UniformVariant& uniform_) {

			std::visit([](auto v) {
				
				
				v.setUniformToShader(); 
			

				}, uniform_);

			});

		glBindVertexArray(VAO);

		list<OglObject3D::SSBOAttribute> extraSSBOAttributes = this->geometry->getExtraSSBOAttributes();
		for_each(extraSSBOAttributes.begin(), extraSSBOAttributes.end(), [](OglObject3D::SSBOAttribute& ssbo) {
			ssbo.Use();
			});

		if (this->personalData["errorMesh"] == "false") {

		glDrawElements(GL_TRIANGLES, geometry->indices.size(), GL_UNSIGNED_INT, 0);
		}
		
		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		int maxAvailableTextureUnits = MyTextureV2::Texture::MaxTextureCount;
		MyTextureV2::Texture::ResetAvailableTextureUnits(maxAvailableTextureUnits);

	}


	//POÝNTS


	Points* Points::clone() {
		RawShaderMaterial* raw = GetMaterial<RawShaderMaterial>();
		MeshBasicMaterial* meshbasic = GetMaterial<MeshBasicMaterial>();
		MeshStandardMaterial* meshstandard = GetMaterial<MeshStandardMaterial>();

		BufferGeometry* buffer = GetGeometry<BufferGeometry>();
		PlaneGeometry* plane = GetGeometry<PlaneGeometry>();
		BoxGeometry* box = GetGeometry<BoxGeometry>();
		SphereGeometry* sphere = GetGeometry<SphereGeometry>();

		Points* cloneMesh = new Points();

		if (raw != nullptr) {
			cloneMesh->material = raw->clone();
		}
		else if (meshbasic != nullptr) {
			cloneMesh->material = meshbasic->clone();
		}
		else if (meshstandard != nullptr) {
			cloneMesh->material = meshstandard->clone();
		}


		if (plane != nullptr) {
			cloneMesh->geometry = plane->clone();
		}
		else if (box != nullptr) {
			cloneMesh->geometry = box->clone();
		}
		else if (sphere != nullptr) {
			cloneMesh->geometry = sphere->clone();
		}
		else if(buffer!=nullptr){
			cloneMesh->geometry = buffer->clone();
		}

		return cloneMesh;
	}

	Points& Points::operator=(const Points& other) {
		if (this != &other) {
			Mesh::operator=(other);
		}
		return *this;
	}
	
	void Points::Render() {

		//compile if not
		if (!this->material->isCompiled()) {
			material->Compile(geometry);
		}

		//binding shader
		material->BindShader();
		material->setUniformLocations();

		
		//binding uniforms
		for_each(material->uniforms.begin(), material->uniforms.end(), [](MaterialUniform::UniformVariant& uniform_) {
			
			std::visit([](auto v) {


				v.setUniformToShader();


				}, uniform_);

			});


	
		glBindVertexArray(VAO);
		list<OglObject3D::SSBOAttribute> extraSSBOAttributes = this->geometry->getExtraSSBOAttributes();
		for_each(extraSSBOAttributes.begin(), extraSSBOAttributes.end(), [](OglObject3D::SSBOAttribute& ssbo) {
			ssbo.Use();
			});
		glEnable(GL_PROGRAM_POINT_SIZE); //now size is set in shader, no need to set in here!!!
		//glEnable(GL_POINT_SPRITE);
		glDrawArrays(GL_POINTS, 0, this->geometry->position.values.size() / 3);
		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		int maxAvailableTextureUnits = MyTextureV2::Texture::MaxTextureCount;
		MyTextureV2::Texture::ResetAvailableTextureUnits(maxAvailableTextureUnits);

	}


	//LINE ------------------------------------------------------------

	Line::Line(Geometry* geo, Material* mat):Mesh(geo,mat){

	}
	Line::Line(Geometry& geo, Material& mat) :Mesh(geo, mat) {

		this->geometry = &geo;
		this->material = &mat;

	}
	Line::Line(const Line& other) :Mesh(){

		this->geometry= other.geometry;
		this->material = other.material;
		this->lineSize = other.lineSize;
	}

	Line& Line::operator=(const Line& other){
		
		this->geometry = other.geometry;
		this->material = other.material;
		this->lineSize = other.lineSize;


		return *this;
	
	}

	void Line::setSize(float size) {

		this->lineSize = size;

	}

	void Line::LoadMesh() {

		this->isLoaded_ = true;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);



		int bufferNumber = geometry->getBufferNumber() + 1;
		if (geometry->indices.size() == 0) {
			int vertexCount = geometry->position.values.size() / 3;
			for (int i = 0; i < vertexCount; i++) {
				this->geometry->indices.push_back(i);
			}
		}



		this->m_Buffers = new GLuint[bufferNumber];
		glGenBuffers(bufferNumber, m_Buffers);

		this->linePositionBuffer = m_Buffers[0];
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
		if (geometry->position.values.size() > 0) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->position.values[0]) * geometry->position.values.size(), &geometry->position.values[0], GL_DYNAMIC_DRAW);
		}
		else {
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, nullptr, GL_DYNAMIC_DRAW);
		}
		glEnableVertexAttribArray(geometry->position.layout_);
		glVertexAttribPointer(geometry->position.layout_, geometry->position.count, GL_FLOAT, GL_FALSE, 0, 0);

		/*
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->texcoord.values[0]) * geometry->texcoord.values.size(), &geometry->texcoord.values[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(geometry->texcoord.layout_);
		glVertexAttribPointer(geometry->texcoord.layout_, geometry->texcoord.count, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(geometry->normal.values[0]) * geometry->normal.values.size(), &geometry->normal.values[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(geometry->normal.layout_);
		glVertexAttribPointer(geometry->normal.layout_, geometry->normal.count, GL_FLOAT, GL_FALSE, 0, 0);

		*/


		list<OglObject3D::BufferAttribute> extraAttributes = geometry->getExtraBufferAttributes();
		auto it = extraAttributes.begin();
		for (int i = 0; i < extraAttributes.size(); i++) {

			if (it->values.size() == 0) {
				it++;
				continue;
			}

			if (it->value_type == "float") {


				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + 3]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(it->values[0]) * it->values.size(), &it->values[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it->layout_);
				glVertexAttribPointer(it->layout_, it->count, GL_FLOAT, GL_FALSE, 0, 0);


			}
			else if (it->value_type == "int") {

				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + 3]);
				vector<int> intValues(it->values.begin(), it->values.end());
				glBufferData(GL_ARRAY_BUFFER, sizeof(intValues[0]) * intValues.size(), &intValues[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it->layout_);
				glVertexAttribIPointer(it->layout_, it->count, GL_INT, 0, 0);
			}
			else if (it->value_type == "uint") {

				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + 3]);
				vector<unsigned int> uintValues(it->values.begin(), it->values.end());
				glBufferData(GL_ARRAY_BUFFER, sizeof(uintValues[0]) * uintValues.size(), &uintValues[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it->layout_);
				glVertexAttribIPointer(it->layout_, it->count, GL_UNSIGNED_INT, 0, 0);
			}


			it++;
		}


		int indexOffset = 3 + geometry->getExtraBufferAttributes().size();
		list<OglObject3D::InstancedAttribute> instancedAttributes = geometry->getInstancedBufferAttributes();
		auto it2 = instancedAttributes.begin();
		for (int i = 0; i < instancedAttributes.size(); i++) {

			if (it2->values.size() == 0) {
				it2++;
				continue;
			}

			if (it2->value_type == "float") {


				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + indexOffset]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(it2->values[0]) * it2->values.size(), &it2->values[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it2->layout_);
				glVertexAttribPointer(it2->layout_, it2->count, GL_FLOAT, GL_FALSE, 0, 0);
				glVertexAttribDivisor(it2->layout_, 1);

			}
			else if (it2->value_type == "int") {

				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + indexOffset]);
				vector<int> intValues(it2->values.begin(), it2->values.end());
				glBufferData(GL_ARRAY_BUFFER, sizeof(intValues[0]) * intValues.size(), &intValues[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it2->layout_);
				glVertexAttribIPointer(it2->layout_, it2->count, GL_INT, 0, 0);
				glVertexAttribDivisor(it2->layout_, 1);
			}
			else if (it2->value_type == "uint") {

				glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[i + indexOffset]);
				vector<unsigned int> uintValues(it2->values.begin(), it2->values.end());
				glBufferData(GL_ARRAY_BUFFER, sizeof(uintValues[0]) * uintValues.size(), &uintValues[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(it2->layout_);
				glVertexAttribIPointer(it2->layout_, it2->count, GL_UNSIGNED_INT, 0, 0);
				glVertexAttribDivisor(it2->layout_, 1);
			}


			it2++;
		}



		list<OglObject3D::SSBOAttribute> extraSSBOAttributes = geometry->getExtraSSBOAttributes();
		auto itSSBO = extraSSBOAttributes.begin();
		for (; itSSBO != extraSSBOAttributes.end(); itSSBO++) {

			itSSBO->Use();
		}


		glBindVertexArray(0);

	}

	void Line::Render() {

		//compile if not
		if (!this->material->isCompiled()) {
			material->Compile(geometry);
		}


		//binding shader
		material->BindShader();
		material->setUniformLocations();

		//binding uniforms
		for_each(material->uniforms.begin(), material->uniforms.end(), [](MaterialUniform::UniformVariant& uniform_) {

			std::visit([](auto v) {


				v.setUniformToShader();


				}, uniform_);

			});


		glBindBuffer(GL_ARRAY_BUFFER, this->linePositionBuffer);
		glBufferData(
			GL_ARRAY_BUFFER,
			this->geometry->position.values.size() * sizeof(geometry->position.values[0]),
			this->geometry->position.values.data(),
			GL_DYNAMIC_DRAW
		);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glBindVertexArray(VAO);

		list<OglObject3D::SSBOAttribute> extraSSBOAttributes = this->geometry->getExtraSSBOAttributes();
		for_each(extraSSBOAttributes.begin(), extraSSBOAttributes.end(), [](OglObject3D::SSBOAttribute& ssbo) {
			ssbo.Use();
			});
		glLineWidth(this->lineSize);
		glDrawArrays(
			GL_LINE_STRIP,
			0,
			static_cast<GLsizei>(this->geometry->position.values.size()/3)
		);

		glBindVertexArray(0);

		int maxAvailableTextureUnits = MyTextureV2::Texture::MaxTextureCount;
		MyTextureV2::Texture::ResetAvailableTextureUnits(maxAvailableTextureUnits);


	}
}