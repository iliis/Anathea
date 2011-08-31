#include "image_mgr.hpp"
#include "tools/stringhelpers.hpp"

#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
/*
 *  0   1         2   3
 *  /---v---------v---\
 *  |   |         |   |
 *  |---|---------|---|
 *         etc.
 */
//------------------------------------------------------------------------------
Vect::T
NinePatchData::getPos(int dim, int p, const Vect& total_size)
const
{
	assert(valid_dim2(dim));
	switch(p)
	{
	case 0:
		return 0;
	case 1:
		return (dim==1?left:top);
	case 2:
		return (dim==1?total_size.x-right:total_size.y-bottom);
	case 3:
		return (dim==1?total_size.x:total_size.y);
	default:
		throw Error("illegalOperation", "NinePatchData::getPos(): invalid dim: "+ToString(dim));
	}
};
//------------------------------------------------------------------------------
/*
 *    0      1      2
 *  /---v---------v---\
 *  |   |         |   |
 *  | --|---------|---|
 *          etc.
 */
//------------------------------------------------------------------------------
Vect::T
NinePatchData::getSize(int dim, int p, const Vect& total_size)
const
{
	assert(valid_dim2(dim));

	switch(p)
	{
	case 0:
		return getPos(dim,1,total_size);
	case 1:
		return getPos(dim,2,total_size)-getPos(dim,1,total_size);
	case 2:
		return (dim==1?right:bottom);
	default:
		throw Error("illegalOperation", "NinePatchData::getSize(): invalid dim: "+ToString(dim));
	}
};
//------------------------------------------------------------------------------
Box
NinePatchData::getPatch(int x, int y, const Box& total)
const
{
	return Box(getPos (1,x,total.size)+total.pos.x, getPos (2,y,total.size)+total.pos.y,
				getSize(1,x,total.size),             getSize(2,y,total.size));
};
//------------------------------------------------------------------------------
NinePatchData
NinePatchData::normalize_to_size(const Vect& size)
const
{
	return
	NinePatchData(this->enabled,
				this->top/size.y,    this->left/size.x,
				this->bottom/size.y, this->right/size.x);
};
//------------------------------------------------------------------------------
const char*
getOpenGLError(GLenum code)
{
	switch(code)
	{
	case GL_INVALID_ENUM:
		return "invalid enum";
	case GL_INVALID_VALUE:
		return "invalid value";
	case GL_INVALID_OPERATION:
		return "invalid operation";
	case GL_STACK_OVERFLOW:
		return "stack overflow";
	case GL_STACK_UNDERFLOW:
		return "stack underflow";
	case GL_OUT_OF_MEMORY:
		return "out of memory";
	case GL_TABLE_TOO_LARGE:
		return "table too large";
	case GL_NO_ERROR:
		return "NO ERROR";
	default:
		return "UNKNOWN ERROR";
	}
};
//------------------------------------------------------------------------------
void
checkOpenGLError()
{
#ifdef DEBUG
	GLenum code = glGetError();
	if(code != GL_NO_ERROR)
		throw Error("opengl", getOpenGLError(code));
#endif
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
GLuint
convertSDL_SurfaceToTexture(SDL_Surface*& surface, const string& filename)
{
	if(!surface)
		throw Error("illegalOperation", "Can't convert SDL_Surface to texture because surface '"+filename+"' is NULL.");

	//see: http://wiki.gamedev.net/index.php/SDL:Tutorials:Using_SDL_with_OpenGL -> How To Load an OpenGL Texture from an SDL_Surface (04.09.08)
	if((!isPowerOfTwo(surface->w) || !isPowerOfTwo(surface->h)) && (((string)((char*)glGetString(GL_EXTENSIONS))).find("ARB_texture_non_power_of_two")) == string::npos)
	{
		/*Der folgende Code soll ein Bild so konvertieren, dass seine Grösse einer Zweierpotenz entspricht, jedoch funktionierts mit der Transparenz noch nicht richtig,
		  und ausserdem ist das ganze nur auf älteren System noch notwenig...

		cout << "WARNING: \"" << filename << "\"'s width/height is not a power of 2, wich is something that's not supported by your system!" << endl << "Trying to convert image..." << endl;
		SDL_Surface* tmp = SDL_CreateRGBSurface(0, nextPowerOfTwo(surface->w), nextPowerOfTwo(surface->h), surface->format->BitsPerPixel, RMASK, GMASK, BMASK, AMASK);

		this->uv_area = Box(0, 0, surface->w/tmp->w, surface->h/tmp->h);

		SDL_SetAlpha(surface, 0, SDL_ALPHA_OPAQUE);
		SDL_SetAlpha(tmp, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

		SDL_FillRect(tmp, 0, SDL_MapRGBA(tmp->format, 255, 255, 255, 255));
		SDL_FillRect(surface, 0, SDL_MapRGBA(surface->format, 0, 255, 0, 5));
		//SDL_BlitSurface(tmp, 0, surface, 0);
		//SDL_SetAlpha(surface, SDL_RLEACCEL, 100);

		SDL_Rect dstrect;
		dstrect.x = 0; dstrect.y = 0;
		dstrect.w = surface->w; dstrect.h = surface->h;

		SDL_BlitSurface(surface, 0, tmp, &dstrect);
		if(surface){SDL_FreeSurface(surface);}
		surface = tmp;*/

		throw Error("notImplemented", "The height/width "+vector2<int>(surface->w, surface->h).print()+" from \""+filename+"\" are not a power of 2 and that's not supported on your system!");
	}

	GLuint texture = 0;
	GLint nOfColors = surface->format->BytesPerPixel;
	GLenum texture_format = 0;

	if(nOfColors < 3)
	{
		SDL_Surface* old = surface;
		surface = SDL_CreateRGBSurface(0,old->w, old->h, 24, RMASK, GMASK, BMASK, 0);
		SDL_BlitSurface(old, NULL, surface, NULL);
		SDL_FreeSurface(old);
		nOfColors = surface->format->BytesPerPixel;
		// cout << "WARGING: Converted Image from palette or something." << endl;
	}

	if(nOfColors == 4)     // contains an alpha channel
	{
		if (surface->format->Rmask == 0xff)	{texture_format = GL_RGBA;}
		else	{texture_format = GL_BGRA;}
	}
	else if (nOfColors == 3)     // no alpha channel
	{
		if (surface->format->Rmask == 0xff)	{texture_format = GL_RGB;}
		else	{texture_format = GL_BGR;}
	}
	else
		cout << "WARNING: Image is not truecolor...  this will probably break.\n";

	// Have OpenGL generate a texture object handle for us
	glGenTextures(1, &texture); checkOpenGLError();

	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set the texture's stretching properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/// disables texture-repetations so that there are no border-artefacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//automatically generate midmaps (OpenGL >= 1.4)
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAPS, GL_TRUE);

	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);

	GLenum errCode;
	if((errCode = glGetError()) != GL_NO_ERROR)
		throw Error("load", "OpenGL caused an error while loading the image \"" + filename + "\":\n"+getOpenGLError(errCode));

	if(!texture)
		throw Error("load", "Couldn't load image \""+filename+"\". Cause unknown.");

	return texture;
};
//------------------------------------------------------------------------------
void
drawPoint(Vect pos, double rad, Color color)
{
	glDisable(GL_TEXTURE_2D);

	glPointSize(rad);
	glColor4f(color.fr(), color.fg(), color.fb(), color.fa());
	glBegin(GL_POINTS);
		glVertex2f(pos.x, pos.y);
	glEnd();

	glEnable(GL_TEXTURE_2D);
};
//------------------------------------------------------------------------------
void
drawLine(Vect start, Vect stop, Color color, double border_with)
{
	glDisable(GL_TEXTURE_2D);

	glLineWidth(border_with);
	glColor4f(color.fr(), color.fg(), color.fb(), color.fa());
	glBegin(GL_LINES);
		glVertex2f(start.x, start.y);
		glVertex2f(stop.x,  stop.y);
	glEnd();

	glEnable(GL_TEXTURE_2D);
};
//------------------------------------------------------------------------------
void
drawRectangle(Box shape, Color color, double border_with, bool fill)
{
	glDisable(GL_TEXTURE_2D);

	glLineWidth(border_with);
	glColor4f(color.fr(), color.fg(), color.fb(), color.fa());
	glBegin((fill?GL_TRIANGLE_FAN:GL_LINE_LOOP));
		glVertex2f(shape.pos.x,              shape.pos.y);
		glVertex2f(shape.pos.x+shape.size.x, shape.pos.y);
		glVertex2f(shape.pos.x+shape.size.x, shape.pos.y+shape.size.y);
		glVertex2f(shape.pos.x,              shape.pos.y+shape.size.y);
	glEnd();

	glEnable(GL_TEXTURE_2D);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Image::Image(string path)
 : UV(0,0,1,1), texture(new GLuint(0)), counter(new int(1)), color(Color(A_OPAQUE,A_OPAQUE,A_OPAQUE,A_OPAQUE))
{
	this->name = path;

	SDL_Surface* tmp = IMG_Load(path.c_str());

	if(!tmp) ///< pointer is NULL ergo the loading failed
		throw Error("load", "Cannot load image '"+path+"': "+SDL_GetError());

	/// convert the loaded image to a screen-compatible format
	this->surface = SDL_DisplayFormatAlpha(tmp); SDL_FreeSurface(tmp);

	if( ! this->surface)
		throw Error("load", "Cannot convert image '"+path+"' to screen-format.");

	*texture = convertSDL_SurfaceToTexture(this->surface);
};
//------------------------------------------------------------------------------
void
Image::operator=(const Image& img)
{
	if(img.counter != this->counter)
	{
		this->release();

		this->counter    = img.counter;
		this->surface    = img.surface;
		this->texture    = img.texture;
		++*counter;
	}

	this->name       = img.name;
	this->UV         = img.UV;
	this->nine_patch = img.nine_patch;
};
//------------------------------------------------------------------------------
void
Image::setUV(Box uv, bool normalized)
{
	this->UV = uv;

	if(!normalized)
	{
		assert(this->surface);
		Vect s = this->getPhysicalSize().cast<Vect::T>();
		UV = Box(uv.pos  / s,
				 uv.size / s);
	}
}
//------------------------------------------------------------------------------
void
Image::drawToSurface(SDL_Surface* dst_img, Vect pos)
{
	assert(dst_img);
	assert(this->surface);

	SDL_Rect dst;
	dst.x = pos.x; dst.y = pos.y;

	SDL_BlitSurface(this->surface, NULL, dst_img, &dst);
};
//------------------------------------------------------------------------------
void
Image::draw(Vect pos, Box uv, bool uvnormal)
{
	assert(this->surface);

	this->draw(Box( pos,
					uv.size * (uvnormal?this->getPhysicalSize().cast<Vect::T>():Vect(1,1))),
				uv, uvnormal);
};
//------------------------------------------------------------------------------
void
Image::draw_gl_quad(Box uv, Box pos)
{
	glTexCoord2f(uv.pos.x,           uv.pos.y);           glVertex2f(pos.pos.x, 				pos.pos.y);
	glTexCoord2f(uv.pos.x+uv.size.x, uv.pos.y);           glVertex2f(pos.pos.x + pos.size.x, 	pos.pos.y);
	glTexCoord2f(uv.pos.x+uv.size.x, uv.pos.y+uv.size.y); glVertex2f(pos.pos.x + pos.size.x, 	pos.pos.y + pos.size.y);
	glTexCoord2f(uv.pos.x,           uv.pos.y+uv.size.y); glVertex2f(pos.pos.x,					pos.pos.y + pos.size.y);
};
//------------------------------------------------------------------------------
void
Image::draw(Box pos, Box uv, bool uvnormal)
{
	if(!this->valid())
		throw Error("null", "Cannot draw Image ('"+this->name+"'): Texture is NULL.");

	if(!uvnormal)
	{
		assert(this->surface);
		uv = Box(	uv.pos  / this->getPhysicalSize().cast<Vect::T>() ,
					uv.size / this->getPhysicalSize().cast<Vect::T>() );
	}

	glBindTexture( GL_TEXTURE_2D, *texture );

	Color c = this->color;
	glBegin( GL_QUADS );
		glColor4f(c.fr(),c.fg(),c.fb(),c.fa());

		if(nine_patch.enabled)
		{
			NinePatchData& np   = this->nine_patch;
			NinePatchData np_uv = np.normalize_to_size(this->getPhysicalSize().cast<Vect::T>());

			for(int x=0;x<3;++x)
			for(int y=0;y<3;++y)
				this->draw_gl_quad(np_uv.getPatch(x,y,uv), np.getPatch(x,y,pos));

			//this->draw_gl_quad(uv, pos, Box(nine_patch.left,0,surface->w-nine_patch.left-nine_patch.right,nine_patch.top));
			//this->draw_gl_quad(uv, pos, Box(surface->w-nine_patch.right,0,nine_patch.right,nine_patch.top));
		}
		else
			this->draw_gl_quad(uv,pos);
	glEnd();
};
//------------------------------------------------------------------------------
void
Image::draw_rotated(Box pos, double angle, Vect center, Box uv, bool uvnormal)
{
	Vect size = this->getSize().cast<Vect::T>();
	if(angle == 0) this->draw(pos.pos-center*size,Box(0,0,1,1),uvnormal);
	else
	{
		if(!this->valid())
			throw Error("null", "Cannot draw Image ('"+this->name+"'): Texture is NULL.");

		if(!uvnormal)
		{
			assert(this->surface);
			uv = Box(	uv.pos  / size ,
						uv.size / size );
		}

		glBindTexture( GL_TEXTURE_2D, *texture );

		center = center * size; ///< center ist relativ
		Vect p1 = Vect(0,0)-center;
		Vect p2 = Vect(pos.size.x, 0) - center;
		Vect p3 = pos.size - center;
		Vect p4 = Vect(0, pos.size.y) - center;
		p1.rotate(angle); p1 = p1 + pos.pos;
		p2.rotate(angle); p2 = p2 + pos.pos;
		p3.rotate(angle); p3 = p3 + pos.pos;
		p4.rotate(angle); p4 = p4 + pos.pos;

		Color c = this->color;
		glBegin( GL_QUADS );
			glColor4f(c.fr(),c.fg(),c.fb(),c.fa());
			glTexCoord2f(uv.pos.x,           uv.pos.y);           glVertex2f(p1.x, p1.y);
			glTexCoord2f(uv.pos.x+uv.size.x, uv.pos.y);           glVertex2f(p2.x, p2.y);
			glTexCoord2f(uv.pos.x+uv.size.x, uv.pos.y+uv.size.y); glVertex2f(p3.x, p3.y);
			glTexCoord2f(uv.pos.x,           uv.pos.y+uv.size.y); glVertex2f(p4.x, p4.y);
		glEnd();
	}
};
//------------------------------------------------------------------------------
void
Image::draw_center(Vect pos, Box uv, bool uvnormal)
{
	this->draw(pos-(this->getSize().cast<Vect::T>()/2), uv, uvnormal);
};
//------------------------------------------------------------------------------
#include "managers/definitions.hpp"
void
Image::drawPart(Box src, Vect pos)
{
	Vect size = this->getSize().cast<Vect::T>();
	Box uv;

	uv.pos  = src.pos /size;
	uv.size = src.size/size;

	this->draw(pos, uv);
};
//------------------------------------------------------------------------------
void
Image::fill(Color col)
{
	if(this->surface)
		SDL_FillRect(this->surface, NULL,
				SDL_MapRGBA(this->surface->format, col.r, col.g, col.b, col.a));

	this->updateTexture();
};
//------------------------------------------------------------------------------
void
Image::updateTexture()
{
	assert(this->surface);

	/// alte löschen
	glDeleteTextures(0, this->texture);

	/// neue erzeugen
	*this->texture = convertSDL_SurfaceToTexture(this->surface, this->name);
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Uint32 mapRGB (Color c, Image i){return SDL_MapRGB (i.getSurface()->format, c.r, c.g, c.b);}
Uint32 mapRGBA(Color c, Image i){return SDL_MapRGBA(i.getSurface()->format, c.r, c.g, c.b, c.a);}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Font::Font(string path, int size) : name(path)
{
	TTF_Font* tmp = TTF_OpenFont(path.c_str(), size);

	if(!tmp)
		throw Error("load", "Cannot load font '"+path+"': "+TTF_GetError());

	/// create a shared_ptr which will automatically delete the font
	/// at the end of its lifetime
	/// oh, and make sure this is called BEFORE TTF_Quit() !
	this->font = TTF_Font_ptr(tmp, TTF_CloseFont);
	this->valid = true;
};
//------------------------------------------------------------------------------
Image
Font::render(string text, Color textCol, Color bgCol, Quality render_quality, bool utf)
{
	assert(this->valid);

	SDL_Surface* rendered = NULL;

	if(utf)
	{
		switch(render_quality)
		{
			case SOLID:
				rendered = TTF_RenderUTF8_Solid(this->font.get(), text.c_str(), textCol.getSDL_Color());
				break;

			case SHADED:
				rendered = TTF_RenderUTF8_Shaded(this->font.get(), text.c_str(), textCol.getSDL_Color(), bgCol.getSDL_Color());
				break;

			case BLENDED: default:
				rendered = TTF_RenderUTF8_Blended(this->font.get(), text.c_str(), textCol.getSDL_Color());
				break;
		}
	}
	else
	{
		switch(render_quality)
		{
			case SOLID:
				rendered = TTF_RenderText_Solid(this->font.get(), text.c_str(), textCol.getSDL_Color());
				break;

			case SHADED:
				rendered = TTF_RenderText_Shaded(this->font.get(), text.c_str(), textCol.getSDL_Color(), bgCol.getSDL_Color());
				break;

			case BLENDED: default:
				rendered = TTF_RenderText_Blended(this->font.get(), text.c_str(), textCol.getSDL_Color());
				break;
		}
	}

	if(rendered == NULL) /// NULL == Error
		throw Error("draw", string("Rendering font failed: ")+string(TTF_GetError())
					+string("\ntext: '")+text
					+string("'\nutf: ")+ToString(utf)+string(" quality: ")+ToString(render_quality));

	/// create a shared_ptr from the rendered SDL_Surface
	return Image(rendered);
};
//------------------------------------------------------------------------------
Image
Font::renderMultiline(string text, boost::shared_ptr<GraphicsManager> graphicsMgr,
					Color textCol,
					Color bgCol, Align align,
					Quality render_quality, bool utf)
{
	/// siehe http://www.gamedev.net/reference/articles/article1960.asp

	assert(this->valid);

	int lineSkip = TTF_FontLineSkip(this->font.get());  ///< Die Zeilenhöhe der Schrift

	/// \a text in einzelne Teile aufteilen:
	list<string> lines = stringToLines(text);

	if(lines.size() == 1)
	{
		return this->render(text, textCol, bgCol, render_quality, utf);
	}
	else
	{
		/// breiteste Zeile finden
		//int width = 0, height = 0;
		VectInt size;
		BOOST_FOREACH(string line, lines)
		{
			int width = 0; if(line == "") continue;
			if(utf)
				TTF_SizeUTF8(this->font.get(), line.c_str(), &width, &size.y); ///< get the width for utf8-text
			else
				TTF_SizeText(this->font.get(), line.c_str(), &width, &size.y); ///< get the width for normal text

			if(width > size.x)
				size.x = width;
		}

		/// wir nehmen an, jede Zeile ist gleich gross...
		size.y = (lines.size()-1) * lineSkip + size.y; ///< plus letzte Zeile

		/// neue Surface erzeugen, auf die dann der text gerendert wird
		Image text_surface = graphicsMgr->createNewImage(size);

		if(render_quality == SHADED)
			text_surface.fill(bgCol);

		int i=0;
		/// den text rendern
		BOOST_FOREACH(string line, lines)
		{
			if(line == "") {++i; continue;}

			Image curline = this->render(line, textCol, bgCol, render_quality, utf);
			SDL_SetAlpha(curline.getSurface(), 0, 0);
			switch(align)
			{
				case LEFT:
					curline.drawToSurface(text_surface.getSurface(), Vect(0, i*lineSkip)); break;
				case CENTER:
					curline.drawToSurface(text_surface.getSurface(), Vect(size.x/2-curline.getPhysicalSize().x/2,
														i*lineSkip)); break;
				case RIGHT:
					curline.drawToSurface(text_surface.getSurface(), Vect(size.x-curline.getPhysicalSize().x,
														i*lineSkip)); break;
			}
			++i;
		}
		text_surface.updateTexture();

		return text_surface;
	}
};
//------------------------------------------------------------------------------
Font::Quality
getFontQualityByName(string s)
{
	if(s == "blended")
		return Font::BLENDED;
	else if(s == "shaded")
		return Font::SHADED;
	else if(s == "solid")
		return Font::SOLID;
	else
		return Font::BLENDED;
};
//------------------------------------------------------------------------------
string
getFontQualityName(Font::Quality q)
{
	switch(q)
	{
		case Font::BLENDED: return "blended";
		case Font::SOLID:   return "solid";
		case Font::SHADED:  return "shaded";
		default: return "blended";
	}
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// sollte ganz am Ende aufgerufen werden, nachdem alle SDL_Surface, TTF_Font etc. gelöscht wurden!
void
quit_sdl_and_ttf()
{
	if(TTF_WasInit())
		TTF_Quit();

	SDL_Quit();
}
//------------------------------------------------------------------------------
void
GraphicsManager::init(vector2<int> size, int bpp, Uint32 flags)
{
	/// SDL selber initialisieren
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
		throw Error("init", "Unable to init SDL: "+string(SDL_GetError()));

	/// SDL-TTF für Schriften initialisieren
	if(!TTF_WasInit() && TTF_Init() == -1)
		throw Error("init", "Unable to init SDL_TTF: "+string(TTF_GetError()));

	/// beim Programmende SDL_Quit sowie evlt. TTF_Quit aufrufen
	atexit(quit_sdl_and_ttf);

	/// OpenGL-doublebuffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

	this->setVideoMode(size, bpp, flags);
	size = this->getScreenSize();

	/// OpenGL-Settings
	glShadeModel(GL_SMOOTH);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH); ///< läuft nicht auf allen Rechnern ;)
	//glEnable(GL_POLYGON_SMOOTH); ///< bewirkt Artefakte mit nouveau-treiber
	glEnable(GL_POINT_SMOOTH);

	glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

	glClearColor(background.fr(), background.fg(), background.fb(), background.fa());

	glViewport(0, 0, size.x, size.y);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0f, size.x, size.y, 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	this->flip();

	/// Notfall-Schrift, am Anfang der Liste, da zuerst geladen
	this->loadFont("fonts/fallback.ttf", 12);

	/// Platzhalter-Bild, für Image() etc.
	this->nullImg = Image("images/null.png");
};
//------------------------------------------------------------------------------
Image
GraphicsManager::loadImage(string path)
{
	/// überprüfe, ob Bild nicht bereits im Speicher ist
	map<string,Image>::iterator it = this->images.find(path);
	if(it != this->images.end())
		return it->second;

	Image newImg(path);
	this->images.insert(pair<string,Image>(path, newImg));
	return newImg;
};
//------------------------------------------------------------------------------
Font
GraphicsManager::loadFont(string path, int size)
{
	/// überprüfe, ob sich Schrift nicht bereits im Speicher befindet
	BOOST_FOREACH(Font f, this->fonts)
		if(f.getSize() == size && f.getPath() == path)
			return f;

	Font newFont(path, size);
	this->fonts.push_back(newFont);
	return newFont;
}
//------------------------------------------------------------------------------
Image
GraphicsManager::createNewImage(vector2<int> size, Uint32 flags)
{
	const SDL_PixelFormat& fmt = *(SDL_GetVideoSurface()->format);

	SDL_Surface* tmp = SDL_CreateRGBSurface(flags,size.x,size.y,
											fmt.BitsPerPixel,
											fmt.Rmask, fmt.Gmask, fmt.Bmask, AMASK); ///< screen hat keinen Alpha-Kanal

	if(tmp == NULL)
		throw Error("create", "Cannot create new SDL-Surface:\n"+string(SDL_GetError())+"\nSize: "+size.print());

	Image newImg(tmp);
	this->images.insert(pair<string,Image>("created image "+ToString(rand()),newImg));
	return newImg;
};
//------------------------------------------------------------------------------
GraphicsManager::~GraphicsManager()
{
	//if(TTF_WasInit())
	//	TTF_Quit();
};
//------------------------------------------------------------------------------
void
GraphicsManager::drawBoxToScreen(Box shape, Color col)
{
	/*rectangleRGBA(this->screen.getSurface().get(),
							shape.pos.x,
							shape.pos.y,
							shape.pos.x+shape.size.x,
							shape.pos.y+shape.size.y,
							col.r, col.g, col.b, col.a);*/
	/// -> SpriteBox::draw()
	glDisable(GL_TEXTURE_2D);

	/// damit die Linien auch schön auf die Pixel passen
	shape.pos = shape.pos.Floor() + Vect(0.5, 0.5);

	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
		glColor4f(col.fr(),col.fg(),col.fb(),col.fa());
		glVertex2f(shape.pos.x,              shape.pos.y);
		glVertex2f(shape.pos.x+shape.size.x, shape.pos.y);
		glVertex2f(shape.pos.x+shape.size.x, shape.pos.y+shape.size.y);
		glVertex2f(shape.pos.x,              shape.pos.y+shape.size.y);
	glEnd();

	glEnable(GL_TEXTURE_2D);
};
//------------------------------------------------------------------------------
void
GraphicsManager::drawLineToScreen(Vect start, Vect end, Color col, double thickness)
{
	glDisable(GL_TEXTURE_2D);

	glLineWidth(thickness);
	glBegin(GL_LINES);
		glColor4f(col.fr(),col.fg(),col.fb(),col.fa());
		glVertex2f(start.x, start.y);
		glVertex2f(end.x,   end.y);
	glEnd();

	glEnable(GL_TEXTURE_2D);
};
//------------------------------------------------------------------------------
Font
GraphicsManager::getFallbackFont()
{
#ifdef DEBUG
	if(this->fonts.empty())
		throw Error("notFound", "FallbackFont not found: GraphicsManager doesn't have any Font at all.");
#endif
	return *this->fonts.begin();
}
//------------------------------------------------------------------------------
void
GraphicsManager::reloadScreen()
{
	this->screen = SDL_GetVideoSurface();
};
//------------------------------------------------------------------------------
void
GraphicsManager::setVideoMode(vector2<int> size, int bpp, Uint32 flags)
{
	/// Grafikmodus setzen
	SDL_Surface* tmp = SDL_SetVideoMode(size.x, size.y, 32, flags);
	if(!tmp)
		throw Error("init", "Unable to set video mode: "+string(SDL_GetError()));

	/// und nen Pointer zum screen speichern; wird von SDL gelöscht
	this->screen = tmp;

	/// OpenGL aktualisieren
	glViewport(0, 0, size.x, size.y);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0f, size.x, size.y, 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	this->screen_width  = screen->w;
	this->screen_height = screen->h;
};
//------------------------------------------------------------------------------
void
GraphicsManager::setScissors(Box s, bool enabled)
{
	this->scissors_enabled = enabled;
	this->scissors = s;
	if(enabled)
	{
		glScissor(GLint(s.pos.x),
				  GLint(this->getScreenSize().y-s.pos.y-s.size.y),
				  GLint(s.size.x), GLint(s.size.y));
		glEnable(GL_SCISSOR_TEST);
	}
	else
		glDisable(GL_SCISSOR_TEST);
};
//------------------------------------------------------------------------------
void
GraphicsManager::disableScissors()
{
	glDisable(GL_SCISSOR_TEST);
	this->scissors_enabled = false;
};
//------------------------------------------------------------------------------
