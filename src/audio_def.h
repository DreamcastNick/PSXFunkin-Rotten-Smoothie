#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(29000)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(7300)},  //XA_GameOver
	//1.XA
	{XA_1, XA_LENGTH(21600)}, //XA_RottenSmoothie
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\1.XA;1", //XA_1
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	//1.XA
	{"rotten-smoothie", true}, //XA_RottenSmoothie
	
	{NULL, false}
};
