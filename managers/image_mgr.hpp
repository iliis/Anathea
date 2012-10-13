#ifndef IMAGE_MGR_HPP_INCLUDED
#define IMAGE_MGR_HPP_INCLUDED

#include <string.h>
#include <list>

#include <boost/shared_ptr.hpp>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
//#include "SDL_gfxPrimitives.h"

#include "tools/errorclass.hpp"
#include "tools/geometry.hpp"
#include "tools/expression.hpp"
#include "managers/definitions.hpp"

using namespace std;
using namespace boost;

struct null_deleter
	{ void operator()(void const *) const{} };

struct NinePatchData
{
	bool enabled;
	Vect::T top,left,bottom,right;

	NinePatchData(bool enabled = false, Vect::T top = 0, Vect::T left = 0, Vect::T bottom = 0, Vect::T right = 0)
		: enabled(enabled), top(top), left(left), bottom(bottom), right(right) {};

	NinePatchData(ptree pt)
		: enabled(getBoolFromPT(pt, "enabled", true)),
		  top   (pt.get<Vect::T>("top", 0)),
		  left  (pt.get<Vect::T>("left", 0)),
		  bottom(pt.get<Vect::T>("bottom", 0)),
		  right (pt.get<Vect::T>("right", 0))
		{}

	NinePatchData normalize_to_size(const Vect& size) const;

	Vect::T getPos (int dim, int p, const Vect& total_size) const;
	Vect::T getSize(int dim, int p, const Vect& total_size) const;
	Box getPatch(int x, int y, const Box& total) const;
};

/// filename ist nur für Fehlermeldungen
GLuint convertSDL_SurfaceToTexture(SDL_Surface*& surface, const string& filename="GENERIC_IMAGE");

void drawPoint(Vect pos, double rad, Color color=WHITE);
void drawLine(Vect start, Vect stop, Color color=WHITE, double border_with=1);
void drawRectangle(Box shape, Color color=WHITE, double border_with=1, bool fill=false);

const char* getOpenGLError(GLenum code);

#define CHECK_GL_ERROR() checkOpenGLError(__FILE__, __LINE__)
void checkOpenGLError(const char* filename, const int line);

//------------------------------------------------------------------------------
/// Eine einfache Klasse, die ein Bild lädt, zeichnet und automatisch wieder freigibt.
/// Ist ein smart pointer, kopieren geht also problemlos.
class Image
{
protected:
	string name;
	NinePatchData nine_patch;
	Box UV;

	GLuint* texture; ///< muss ein Pointer sein, damit Textur aktualisiert werden kann
	SDL_Surface* surface;

	int* counter;

	void release()
	{
		(*counter) --;
		if(*counter == 0)
		{
			SDL_FreeSurface(surface);
			glDeleteTextures(1, texture);

			delete counter; *texture = 0;
			delete texture;
		}

		counter = NULL;
		texture = NULL;
		surface = NULL;
	}

	void draw_gl_quad(Box uv, Box pos);
public:
	Color color;


	/// lädt das Bild
	/// @param path gibt den Pfad zum Bild an
	Image(string path);

	/// übernimmt ein bereits existierendes Bild
	/// @param srf shared-pointer auf eine SDL-Surface
	Image(SDL_Surface* srf)
	 : name("from SDL_Surface"), UV(0,0,1,1), texture(new GLuint(0)), surface(srf), counter(new int(1)), color(Color(A_OPAQUE,A_OPAQUE,A_OPAQUE,A_OPAQUE))
		{if(srf){*texture = convertSDL_SurfaceToTexture(surface,name);}}
		//else throw ERROR("load", "Cannot create Image from SDL_Surface: NULL");}; /// für Screen etc.

	Image(const Image& i)
	 : name(i.name), nine_patch(i.nine_patch), UV(i.UV), texture(i.texture), surface(i.surface), counter(i.counter), color(i.color)
		{++*counter;}

	virtual ~Image(){release();};

	void operator=(const Image& img);

	/** zeichnet das Bild auf ein anderes
	  * Achtung: updateTexture nicht vergessen!
	  * @param img Das Ziel. Das Opfer. Darauf wird gezeichnet.
	  * @param pos Dorthin wird auf img gezeichnet.
	  */
	void drawToSurface(SDL_Surface* dest_img, Vect pos);

	/// zeichnet texture auf den Bildschirm
	inline
	void draw(Vect pos){this->draw(pos,this->UV,true);}
	inline
	void draw(Box  pos){this->draw(pos,this->UV,true);}
	void draw(Vect pos, Box uv, bool uvnormal=true);
	void draw( Box pos, Box uv, bool uvnormal=true); ///< uvnormal=true: uv=0-1, sonst uv=0-imgsize

	/// zeichnet Bild mit Mitte bei pos
	void draw_center(Vect pos, Box uv=Box(0,0,1,1), bool uvnormal=true);

	/// rotiert Bild um center
	void draw_rotated(Box pos, double angle, Vect center=Vect(0,0), Box uv=Box(0,0,1,1), bool uvnormal=true);
	inline void draw_rotated(Vect pos, double angle, Vect center=Vect(0,0), Box uv=Box(0,0,1,1), bool uvnormal=true)
		{draw_rotated(Box(pos, this->getSize().cast<Vect::T>()), angle, center, uv, uvnormal);};

	/** zeichnet ein Teil des Bildes.
	  * @param src Der Ausschnitt des Bildes der gezeichnet werden soll.
	  * @param pos Dorthin wird auf img gezeichnet.
	  */
	void drawPart(Box src, Vect pos);

	/// aktualisiert OGL-Textur mit der eigenen surface
	void updateTexture();

	/// Löscht den Inhalt und überschreibt ihn mit einer Farbe.
	void fill(Color col);

	/** Gibt den SDL-Pointer zurück. Nicht löschen!
	  * Dieser Pointer wird automatisch freigegeben. */
	inline SDL_Surface* getSurface(){return this->surface;}

	/** Gibt die OpenGL-Textur zurück. Nicht löschen! */
	inline GLuint getTexture(){return *this->texture;}

	inline bool valid(){return this->surface != NULL and this->texture > 0;}

	/// Ermittelt die Grösse des Bildes.
	Vect                 getSize()          const;
	inline vector2<int> getPhysicalSize()  const {return vector2<int>(this->surface->w, this->surface->h);}

	/// gibt den Pfad zurück, falls das Bild nicht von einer existierenenden SDL_Surface erzeugt wurde
	inline const string getName() const {return this->name;}

	/// kopiert ein Bild
	Image createCopy(){throw ERROR("notImplemented", "Image::createCopy()");}

	/// NinePatch-Einstellungen
	/// Zeichnet ein Bild entweder normal (nd.enabled = false)
	/// oder zeichnet es mit ungestrecktem Rahmen
	/// (siehe z.B. http://developer.android.com/guide/topics/graphics/2d-graphics.html#nine-patch)
	inline void setNinePatchData(NinePatchData nd){this->nine_patch = nd;}
	inline NinePatchData NinePatch() const {return this->nine_patch;}

	/// UV-Koordinaten: Welche Teile der Grafik genau genommen werden
	/// Nützlich z.B. um mehrere Bilder in einer Datei zu kombinieren
	void         setUV(Box uv, bool normalized=false);
	inline Box   getUV() const {return this->UV;}

	/// Setze Parameter (UV, NinePatch, ...) via PTree
	void         set(ptree pt);

	/// Direkter Pointer zur SDL_Surface. Vorsichtig verwenden! Wird nach wie vor von Image verwaltet.
	inline SDL_Surface* getSurface() const {return this->surface;}
};

/// operator<< has to be in namespace boost for boost::variant
namespace boost
{
	inline std::ostream& operator<<(std::ostream& os, const Image& i)
	{
		os << i.getName();
		return os;
	};
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Uint32 mapRGB (Color c, Image i);
Uint32 mapRGBA(Color c, Image i);
//------------------------------------------------------------------------------

class GraphicsManager;

//------------------------------------------------------------------------------
typedef boost::shared_ptr<TTF_Font> TTF_Font_ptr;
/// Lädt, speichert, rendert und löscht eine Font.
class Font
{
	bool valid;
	TTF_Font_ptr font;
	int size;
	string name;
public:
	enum Quality
	{
		SOLID, ///< Schnell und hässlich, kein AA, transparenter Hintergrund
		SHADED, ///< Relativ Schnell und schön, mit AA, farbiger Hintergrund
		BLENDED ///< Langsam und schön, mit AA, transparenter Hintergrund
	};

	/** lädt eine Font
	  * @param path Pfad zur ttf-Datei.
	  * @param size Schriftgrösse in Punkt (=Pixel?)
	  */
	Font(string path, int size);

	Font() : valid(false), size(0), name("NULL") {};

	/**
	 * Rendert einen \a text auf eine Surface (Image)
	 * @param text Der Text, der gerendert werden soll.
	 * @param textCol Die Farbe des Textes.
	 * @param textAlpha Transparenz des Textes.
	 * @param bgCol Hintergrundfarbe, nur bei SHADED relevant.
	 * @param bgAlpha Transparenz des Hintergrundes, nur bei SHADED relevant.
	 * @param render_quality Die Qualität, SOLID, SHADED oder BLENDED, siehe enum Quality
	 * @param utf Ob der \a text in UTF8 oder "normal" gerendert werden soll.
	 */
	Image render(string text,
					Color textCol,
					Color bgCol,
					Quality render_quality=BLENDED, bool utf=false);

	Image renderMultiline(string text, boost::shared_ptr<GraphicsManager> graphicsMgr,
					Color textCol,
					Color bgCol,
					Align align=LEFT,
					Quality render_quality=BLENDED, bool utf=false);

	/// Gibt an, wieviele Punkt (= Pixel ?) die geladene Font hat.
	inline int getSize(){return this->size;}

	/// Gibt den Pfad zur Schrift-Datei ;)
	const string getPath() const {return this->name;}

	inline bool isValid(){return this->valid;}
};

/// versucht einen string in eine Font::Quality zu konvertieren
Font::Quality getFontQualityByName(string s);
string        getFontQualityName(Font::Quality q);

/// operator<< has to be in namespace boost for boost::variant
namespace boost
{
	inline std::ostream& operator<<(std::ostream& os, const Font& f)
	{
		os << f.getPath();
		return os;
	}
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Speichert von jedem Bild und Font eine Instanz und ist ausserdem zuständig für grundlegende Grafikoptionen.
class GraphicsManager
{
	map<string, Image> images; 	///< alle mit diesem Manager geladene Bilder
	list<Font>  fonts;

	SDL_Surface *screen; 	///< Der Bildschirm. Was sonst? Nespresso etwa?
	Image nullImg;			///< Platzhalter, für leere Bilder etc.

	Color background;		///< Hintergrundfarbe

	Box scissors; bool scissors_enabled;

	/// lädt Bildschirm neu, z.B. bei Fenstergrössenänderungen
	void	reloadScreen();
public:
	GraphicsManager() : screen(NULL), nullImg(NULL), background(0,0,0,0,0), scissors_enabled(false) {};
	~GraphicsManager();

	/// initialisiert SDL und öffnet ein Grafik-Fenster (oder gerade den ganzen Bildschirm)
	void	init(vector2<int> size, int bpp=32, Uint32 flags = SDL_DOUBLEBUF | SDL_FULLSCREEN);

	/// lädt ein Bild (sofern noch nicht geladen) und speichert es intern
	Image	loadImage(string path);

	/// lädt ein Bild anhand eines PTrees ("path", ev. "UV", "Color", "NinePatch")
	Image   loadImage(ptree data);

	/// lädt eine Schrift und speichert sie
	Font	loadFont(string path, int size);

	/// erzeugt ein neues, leeres Bild
	Image	createNewImage(vector2<int> size, Uint32 flags = 0);

	/// erzeugt einen Platzhalter
	inline
	Image&	getFallbackImage(){return nullImg;};

	Font	getFallbackFont();

	/// zeichnet ein Bild auf den Bildschirm
	//void	drawToScreen(Image img, vector2<int> pos){img.drawTo(this->screen, pos);};

	/// zeichnet ein Rechteck auf den Bildschirm (z.B. für Bounding-Boxes)
	void	drawBoxToScreen(Box shape, Color col=WHITE);

	/// zeichnet eine Linie ;)
	void	drawLineToScreen(Vect start, Vect end, Color col=WHITE, double thickness=1);

	inline SDL_Surface* getScreen() {return this->screen;}
	inline VectInt getScreenSize() {return VectInt(this->screen->w, this->screen->h);}

	Expression<FNumber> screen_width, screen_height;

	/// tauscht front- und backbuffer
	void	flip(){SDL_GL_SwapBuffers();}

	/// löscht den Bildschirm mit Hintergrundfarbe
	void	cls(){glClear(GL_COLOR_BUFFER_BIT);}

	void	setBackgroundColor(Color bg){this->background = bg; glClearColor(bg.fr(), bg.fg(), bg.fb(), bg.fa());}
	inline Color getBackgroundColor() const {return this->background;}

	void	setVideoMode(vector2<int> size, int bpp=32, Uint32 flags = SDL_OPENGL | SDL_FULLSCREEN);

	/// begrenzt zeichnen auf einen Teil des Bildschirmes
	void    setScissors(Box s, bool enabled = true);
	void    disableScissors();
	bool    scissorsEnabled(){return this->scissors_enabled;}
	Box     getScissors(){return this->scissors;}
};
//------------------------------------------------------------------------------

#endif // IMAGE_MGR_HPP_INCLUDED
