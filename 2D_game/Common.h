typedef struct
{
    float x, y;
} Vertex;

typedef struct{
    Vertex v1;
    Vertex v2;
    float distance;
    int color;
} circle;
typedef struct
{
    Vertex v1;
    Vertex v2;
    int color;
    int hit;
} line;

void init_graphics();
void resize_graphics(uint width, uint height);

void draw_graphics(size_t num_aliens, circle* CrlSegments, size_t num_bombs, circle* bombs, size_t num_missile, line* LineSegments, float currentpos, float currenty, float ang);
