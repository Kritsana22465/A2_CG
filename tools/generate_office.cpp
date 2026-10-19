#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <algorithm>

// ----------------------------------------------------------------------------
// Simple Math & Geometry Structures
// ----------------------------------------------------------------------------
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Vec2 {
    float u, v;
    Vec2() : u(0), v(0) {}
    Vec2(float u, float v) : u(u), v(v) {}
};

struct UVBox {
    float u0, v0, u1, v1;
};

// ----------------------------------------------------------------------------
// 2048x2048 High-Definition Texture Canvas & Drawing Tools
// ----------------------------------------------------------------------------
const int TEX_W = 2048;
const int TEX_H = 2048;

struct Image {
    std::vector<uint8_t> data;
    Image() : data(TEX_W * TEX_H * 3, 255) {}

    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        if (x < 0 || x >= TEX_W || y < 0 || y >= TEX_H) return;
        int idx = (y * TEX_W + x) * 3;
        data[idx + 0] = r;
        data[idx + 1] = g;
        data[idx + 2] = b;
    }

    void fillRect(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
        int minX = std::max(0, std::min(x0, x1));
        int maxX = std::min(TEX_W - 1, std::max(x0, x1));
        int minY = std::max(0, std::min(y0, y1));
        int maxY = std::min(TEX_H - 1, std::max(y0, y1));
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                setPixel(x, y, r, g, b);
            }
        }
    }

    void drawHLine(int x0, int x1, int y, uint8_t r, uint8_t g, uint8_t b) {
        fillRect(x0, y, x1, y, r, g, b);
    }

    void drawVLine(int x, int y0, int y1, uint8_t r, uint8_t g, uint8_t b) {
        fillRect(x, y0, x, y1, r, g, b);
    }

    void drawRectOutline(int x0, int y0, int x1, int y1, int thickness, uint8_t r, uint8_t g, uint8_t b) {
        for (int t = 0; t < thickness; ++t) {
            drawHLine(x0 + t, x1 - t, y0 + t, r, g, b);
            drawHLine(x0 + t, x1 - t, y1 - t, r, g, b);
            drawVLine(x0 + t, y0 + t, y1 - t, r, g, b);
            drawVLine(x1 - t, y0 + t, y1 - t, r, g, b);
        }
    }

    void fillCircle(int cx, int cy, int radius, uint8_t r, uint8_t g, uint8_t b) {
        int r2 = radius * radius;
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                if (dx * dx + dy * dy <= r2) {
                    setPixel(cx + dx, cy + dy, r, g, b);
                }
            }
        }
    }

    void drawRing(int cx, int cy, int rInner, int rOuter, uint8_t r, uint8_t g, uint8_t b) {
        int rIn2 = rInner * rInner;
        int rOut2 = rOuter * rOuter;
        for (int dy = -rOuter; dy <= rOuter; ++dy) {
            for (int dx = -rOuter; dx <= rOuter; ++dx) {
                int d2 = dx * dx + dy * dy;
                if (d2 >= rIn2 && d2 <= rOut2) {
                    setPixel(cx + dx, cy + dy, r, g, b);
                }
            }
        }
    }

    void drawChar5x7(int x, int y, char c, int scale, uint8_t r, uint8_t g, uint8_t b) {
        static const uint8_t font[96][5] = {
            {0x00,0x00,0x00,0x00,0x00}, // ' '
            {0x00,0x00,0x5F,0x00,0x00}, // '!'
            {0x00,0x07,0x00,0x07,0x00}, // '"'
            {0x14,0x7F,0x14,0x7F,0x14}, // '#'
            {0x24,0x2A,0x7F,0x2A,0x12}, // '$'
            {0x23,0x13,0x08,0x64,0x62}, // '%'
            {0x36,0x49,0x55,0x22,0x50}, // '&'
            {0x00,0x05,0x03,0x00,0x00}, // '''
            {0x00,0x1C,0x22,0x41,0x00}, // '('
            {0x00,0x41,0x22,0x1C,0x00}, // ')'
            {0x08,0x2A,0x1C,0x2A,0x08}, // '*'
            {0x08,0x08,0x3E,0x08,0x08}, // '+'
            {0x00,0x50,0x30,0x00,0x00}, // ','
            {0x08,0x08,0x08,0x08,0x08}, // '-'
            {0x00,0x60,0x60,0x00,0x00}, // '.'
            {0x20,0x10,0x08,0x04,0x02}, // '/'
            {0x3E,0x51,0x49,0x45,0x3E}, // '0'
            {0x00,0x42,0x7F,0x40,0x00}, // '1'
            {0x42,0x61,0x51,0x49,0x46}, // '2'
            {0x21,0x41,0x45,0x4B,0x31}, // '3'
            {0x18,0x14,0x12,0x7F,0x10}, // '4'
            {0x27,0x45,0x45,0x45,0x39}, // '5'
            {0x3C,0x4A,0x49,0x49,0x30}, // '6'
            {0x01,0x71,0x09,0x05,0x03}, // '7'
            {0x36,0x49,0x49,0x49,0x36}, // '8'
            {0x06,0x49,0x49,0x29,0x1E}, // '9'
            {0x00,0x36,0x36,0x00,0x00}, // ':'
            {0x00,0x56,0x36,0x00,0x00}, // ';'
            {0x08,0x14,0x22,0x41,0x00}, // '<'
            {0x14,0x14,0x14,0x14,0x14}, // '='
            {0x00,0x41,0x22,0x14,0x08}, // '>'
            {0x02,0x01,0x51,0x09,0x06}, // '?'
            {0x32,0x49,0x79,0x41,0x3E}, // '@'
            {0x7E,0x11,0x11,0x11,0x7E}, // 'A'
            {0x7F,0x49,0x49,0x49,0x36}, // 'B'
            {0x3E,0x41,0x41,0x41,0x22}, // 'C'
            {0x7F,0x41,0x41,0x22,0x1C}, // 'D'
            {0x7F,0x49,0x49,0x49,0x41}, // 'E'
            {0x7F,0x09,0x09,0x09,0x01}, // 'F'
            {0x3E,0x41,0x49,0x49,0x7A}, // 'G'
            {0x7F,0x08,0x08,0x08,0x7F}, // 'H'
            {0x00,0x41,0x7F,0x41,0x00}, // 'I'
            {0x20,0x40,0x41,0x3F,0x01}, // 'J'
            {0x7F,0x08,0x14,0x22,0x41}, // 'K'
            {0x7F,0x40,0x40,0x40,0x40}, // 'L'
            {0x7F,0x02,0x0C,0x02,0x7F}, // 'M'
            {0x7F,0x04,0x08,0x10,0x7F}, // 'N'
            {0x3E,0x41,0x41,0x41,0x3E}, // 'O'
            {0x7F,0x09,0x09,0x09,0x06}, // 'P'
            {0x3E,0x41,0x51,0x21,0x5E}, // 'Q'
            {0x7F,0x09,0x19,0x29,0x46}, // 'R'
            {0x46,0x49,0x49,0x49,0x31}, // 'S'
            {0x01,0x01,0x7F,0x01,0x01}, // 'T'
            {0x3F,0x40,0x40,0x40,0x3F}, // 'U'
            {0x1F,0x20,0x40,0x20,0x1F}, // 'V'
            {0x3F,0x40,0x38,0x40,0x3F}, // 'W'
            {0x63,0x14,0x08,0x14,0x63}, // 'X'
            {0x07,0x08,0x70,0x08,0x07}, // 'Y'
            {0x61,0x51,0x49,0x45,0x43}, // 'Z'
        };

        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
        int ci = c - 32;
        if (ci < 0 || ci >= 96) return;

        for (int col = 0; col < 5; ++col) {
            uint8_t line = font[ci][col];
            for (int row = 0; row < 7; ++row) {
                if (line & (1 << row)) {
                    fillRect(x + col * scale, y + row * scale,
                             x + (col + 1) * scale - 1, y + (row + 1) * scale - 1,
                             r, g, b);
                }
            }
        }
    }

    void drawString(int x, int y, const std::string& str, int scale, uint8_t r, uint8_t g, uint8_t b) {
        int curX = x;
        for (char c : str) {
            drawChar5x7(curX, y, c, scale, r, g, b);
            curX += 6 * scale;
        }
    }

    bool saveBMP(const std::string& filename) {
        std::ofstream out(filename, std::ios::binary);
        if (!out) return false;

        uint32_t rowSize = (TEX_W * 3 + 3) & ~3;
        uint32_t dataSize = rowSize * TEX_H;
        uint32_t fileSize = 54 + dataSize;

        uint8_t fileHeader[14] = {
            'B', 'M',
            (uint8_t)(fileSize & 0xFF), (uint8_t)((fileSize >> 8) & 0xFF),
            (uint8_t)((fileSize >> 16) & 0xFF), (uint8_t)((fileSize >> 24) & 0xFF),
            0, 0, 0, 0,
            54, 0, 0, 0
        };

        uint8_t infoHeader[40] = {
            40, 0, 0, 0,
            (uint8_t)(TEX_W & 0xFF), (uint8_t)((TEX_W >> 8) & 0xFF),
            (uint8_t)((TEX_W >> 16) & 0xFF), (uint8_t)((TEX_W >> 24) & 0xFF),
            (uint8_t)(TEX_H & 0xFF), (uint8_t)((TEX_H >> 8) & 0xFF),
            (uint8_t)((TEX_H >> 16) & 0xFF), (uint8_t)((TEX_H >> 24) & 0xFF),
            1, 0,
            24, 0,
            0, 0, 0, 0,
            (uint8_t)(dataSize & 0xFF), (uint8_t)((dataSize >> 8) & 0xFF),
            (uint8_t)((dataSize >> 16) & 0xFF), (uint8_t)((dataSize >> 24) & 0xFF),
            0x13, 0x0B, 0, 0,
            0x13, 0x0B, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };

        out.write((char*)fileHeader, 14);
        out.write((char*)infoHeader, 40);

        std::vector<uint8_t> row(rowSize, 0);
        for (int y = 0; y < TEX_H; ++y) {
            for (int x = 0; x < TEX_W; ++x) {
                int srcIdx = (y * TEX_W + x) * 3;
                row[x * 3 + 0] = data[srcIdx + 2]; // B
                row[x * 3 + 1] = data[srcIdx + 1]; // G
                row[x * 3 + 2] = data[srcIdx + 0]; // R
            }
            out.write((char*)row.data(), rowSize);
        }

        return true;
    }
};

// ----------------------------------------------------------------------------
// OBJ Mesh Builder
// ----------------------------------------------------------------------------
struct OBJBuilder {
    std::vector<Vec3> vertices;
    std::vector<Vec2> uvs;
    std::vector<Vec3> normals;

    struct FaceVertex {
        int v, vt, vn;
    };
    std::vector<std::vector<FaceVertex>> faces;

    int addVertex(Vec3 p) {
        vertices.push_back(p);
        return (int)vertices.size();
    }

    int addUV(Vec2 t) {
        uvs.push_back(t);
        return (int)uvs.size();
    }

    int addNormal(Vec3 n) {
        normals.push_back(n);
        return (int)normals.size();
    }

    void addQuad(Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3,
                 Vec2 uv0, Vec2 uv1, Vec2 uv2, Vec2 uv3,
                 Vec3 n) {
        int iv0 = addVertex(p0);
        int iv1 = addVertex(p1);
        int iv2 = addVertex(p2);
        int iv3 = addVertex(p3);

        int it0 = addUV(uv0);
        int it1 = addUV(uv1);
        int it2 = addUV(uv2);
        int it3 = addUV(uv3);

        int in = addNormal(n);

        faces.push_back({ {iv0, it0, in}, {iv1, it1, in}, {iv2, it2, in} });
        faces.push_back({ {iv0, it0, in}, {iv2, it2, in}, {iv3, it3, in} });
    }

    void addBox(float minX, float minY, float minZ,
                float maxX, float maxY, float maxZ,
                UVBox uv, bool insideNormals = false) {
        float s = insideNormals ? -1.0f : 1.0f;
        Vec2 uv00(uv.u0, uv.v0);
        Vec2 uv10(uv.u1, uv.v0);
        Vec2 uv11(uv.u1, uv.v1);
        Vec2 uv01(uv.u0, uv.v1);

        if (!insideNormals) {
            // Front (+Z)
            addQuad(Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ),
                    uv00, uv10, uv11, uv01, Vec3(0, 0, 1.0f * s));
            // Back (-Z)
            addQuad(Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ),
                    uv00, uv10, uv11, uv01, Vec3(0, 0, -1.0f * s));
            // Left (-X)
            addQuad(Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Vec3(minX, maxY, minZ),
                    uv00, uv10, uv11, uv01, Vec3(-1.0f * s, 0, 0));
            // Right (+X)
            addQuad(Vec3(maxX, minY, maxZ), Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ),
                    uv00, uv10, uv11, uv01, Vec3(1.0f * s, 0, 0));
            // Top (+Y)
            addQuad(Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ),
                    uv00, uv10, uv11, uv01, Vec3(0, 1.0f * s, 0));
            // Bottom (-Y)
            addQuad(Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ),
                    uv00, uv10, uv11, uv01, Vec3(0, -1.0f * s, 0));
        } else {
            addQuad(Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ),
                    uv00, uv10, uv11, uv01, Vec3(0, 0, 1.0f));
            addQuad(Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ),
                    uv00, uv10, uv11, uv01, Vec3(0, 0, -1.0f));
            addQuad(Vec3(minX, minY, maxZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ),
                    uv00, uv10, uv11, uv01, Vec3(1.0f, 0, 0));
            addQuad(Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, maxY, minZ),
                    uv00, uv10, uv11, uv01, Vec3(-1.0f, 0, 0));
            addQuad(Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ),
                    uv00, uv10, uv11, uv01, Vec3(0, -1.0f, 0));
            addQuad(Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ),
                    uv00, uv10, uv11, uv01, Vec3(0, 1.0f, 0));
        }
    }

    void addCylinder(float cx, float cz, float minY, float maxY, float radius, int segments, UVBox uv) {
        for (int i = 0; i < segments; ++i) {
            float a0 = (float)i / segments * 2.0f * 3.14159265f;
            float a1 = (float)(i + 1) / segments * 2.0f * 3.14159265f;

            float x0 = cx + radius * std::cos(a0);
            float z0 = cz + radius * std::sin(a0);
            float x1 = cx + radius * std::cos(a1);
            float z1 = cz + radius * std::sin(a1);

            Vec3 n0(std::cos(a0), 0, std::sin(a0));
            Vec3 n1(std::cos(a1), 0, std::sin(a1));
            Vec3 nMid((n0.x + n1.x) * 0.5f, 0, (n0.z + n1.z) * 0.5f);

            float uA = uv.u0 + (float)i / segments * (uv.u1 - uv.u0);
            float uB = uv.u0 + (float)(i + 1) / segments * (uv.u1 - uv.u0);

            addQuad(Vec3(x0, minY, z0), Vec3(x1, minY, z1), Vec3(x1, maxY, z1), Vec3(x0, maxY, z0),
                    Vec2(uA, uv.v0), Vec2(uB, uv.v0), Vec2(uB, uv.v1), Vec2(uA, uv.v1),
                    nMid);
        }
    }

    bool saveOBJ(const std::string& filename) {
        std::ofstream out(filename);
        if (!out) return false;

        out << "# Procedural Tech Office Scene Room with AI Workstation Battlestations\n";
        out << "o OfficeSceneRoom\n\n";

        for (const auto& v : vertices) {
            out << "v " << v.x << " " << v.y << " " << v.z << "\n";
        }
        out << "\n";

        for (const auto& t : uvs) {
            out << "vt " << t.u << " " << t.v << "\n";
        }
        out << "\n";

        for (const auto& n : normals) {
            out << "vn " << n.x << " " << n.y << " " << n.z << "\n";
        }
        out << "\n";

        for (const auto& f : faces) {
            out << "f";
            for (const auto& fv : f) {
                out << " " << fv.v << "/" << fv.vt << "/" << fv.vn;
            }
            out << "\n";
        }

        return true;
    }
};

// ----------------------------------------------------------------------------
// Clean up old retro screens from Models/ai_kneeling_workers.obj
// ----------------------------------------------------------------------------
void cleanAIWorkersOBJ(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "[cleanAIWorkersOBJ] Could not open " << path << "\n";
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    bool inOldScreens = false;
    int modifiedFaces = 0;

    while (std::getline(in, line)) {
        if (line.rfind("g Console_Screen", 0) == 0 || line.rfind("g Console_Key", 0) == 0) {
            inOldScreens = true;
        } else if (line.rfind("g KneelingAI", 0) == 0 || line.rfind("g Console_Main", 0) == 0 || line.rfind("g Console_Keyboard", 0) == 0) {
            inOldScreens = false;
        }

        if (inOldScreens && line.rfind("f ", 0) == 0) {
            lines.push_back("# " + line);
            modifiedFaces++;
        } else {
            lines.push_back(line);
        }
    }
    in.close();

    std::ofstream out(path);
    for (const auto& l : lines) {
        out << l << "\n";
    }
    std::cout << "[cleanAIWorkersOBJ] Cleaned " << modifiedFaces << " old console face lines from " << path << "\n";
}

int main() {
    std::cout << "[generate_office] Generating textures and upgrading AI Worker Battlestation Computers...\n";

    // Clean old primitive screens from ai_kneeling_workers.obj
    cleanAIWorkersOBJ("Models/ai_kneeling_workers.obj");

    Image img;

    // ========================================================================
    // UV Layout Map (2048x2048)
    // [0..1023, 0..1023]       : Brown Wood Parquet Flooring (0.0..0.5, 0.0..0.5)
    // [1024..2047, 0..1023]    : Agile Scrum Whiteboard (0.5..1.0, 0.0..0.5)
    // [0..1023, 1024..2047]    : Panoramic Window City Skyline View (0.0..0.5, 0.5..1.0)
    // [1024..1279, 1024..1535] : Primary Monitor (VS Code IDE HD) (0.50..0.625, 0.50..0.75)
    // [1280..1535, 1024..1535] : Secondary Monitor (System Telemetry & Graphs) (0.625..0.75, 0.50..0.75)
    // [1536..1791, 1024..1279] : PC Case Front Mesh with 3 Glowing RGB Fans (0.75..0.875, 0.50..0.625)
    // [1792..2047, 1024..1279] : PC Case Tempered Glass Side Window (GPU/RAM/Liquid Cooler) (0.875..1.0, 0.50..0.625)
    // [1536..1791, 1280..1535] : Custom Mechanical Keyboard (0.75..0.875, 0.625..0.75)
    // [1792..2047, 1280..1535] : Studio Monitor Speakers (0.875..1.0, 0.625..0.75)
    // [1024..1279, 1536..2047] : Clean Office Wooden Door (0.50..0.625, 0.75..1.0) (NO PLAQUE!)
    // [1280..1407, 1536..1663] : Office Wall Clock (0.625..0.6875, 0.75..0.8125)
    // [1280..1535, 1664..2047] : Tech Vision Poster (0.625..0.75, 0.8125..1.0)
    // [1536..1791, 1536..1791] : Bookshelf Spines & Binders (0.75..0.875, 0.75..0.875)
    // [1792..2047, 1536..1791] : Dark Executive Desk Wood (0.875..1.0, 0.75..0.875)
    // [1536..1791, 1792..1919] : Desk Mat Surface (0.75..0.875, 0.875..0.9375)
    // [1792..2047, 1792..1855] : Aluminum Trim (0.875..1.0, 0.875..0.90625)
    // [1792..1919, 1856..1983] : Plant Foliage (0.875..0.9375, 0.90625..0.96875)
    // [1920..2047, 1856..1983] : Drywall Tint (0.9375..1.0, 0.90625..0.96875)
    // [1536..1791, 1920..2047] : Acoustic Ceiling Grid (0.75..0.875, 0.9375..1.0)
    // [1792..2047, 1984..2047] : Brushed Metallic Steel (0.875..1.0, 0.96875..1.0)
    // ========================================================================

    // ------------------------------------------------------------------------
    // 1. Brown Wood Parquet Flooring [0..1023, 0..1023] (Seamless Wood Planks)
    // ------------------------------------------------------------------------
    img.fillRect(0, 0, 1023, 1023, 140, 105, 75);
    for (int y = 0; y < 1024; y += 64) {
        int offset = (y / 64) % 2 == 0 ? 0 : 64;
        for (int x = -128; x <= 1024 + 128; x += 128) {
            int px0 = x + offset + 2;
            int px1 = px0 + 124;
            int py0 = y + 2;
            int py1 = y + 62;

            uint8_t baseR = 195 + (((x + 256) * 19 + y * 37) % 25);
            uint8_t baseG = (uint8_t)(baseR * 0.79f);
            uint8_t baseB = (uint8_t)(baseR * 0.58f);

            img.fillRect(px0, py0, px1, py1, baseR, baseG, baseB);

            for (int gy = py0 + 12; gy < py1; gy += 14) {
                uint8_t grR = baseR - 10;
                uint8_t grG = (uint8_t)(grR * 0.79f);
                uint8_t grB = (uint8_t)(grR * 0.58f);
                img.drawHLine(px0 + 4, px1 - 4, gy, grR, grG, grB);
            }
            img.drawRectOutline(px0, py0, px1, py1, 2, 120, 90, 62);
        }
    }

    // ------------------------------------------------------------------------
    // 2. Agile Scrum Whiteboard [1024..2047, 0..1023]
    // Clean stylized graphic rectangle bars ("ขีดสี่เหลี่ยม") as requested!
    // ------------------------------------------------------------------------
    img.fillRect(1024, 0, 2047, 1023, 248, 250, 252);
    img.drawRectOutline(1024, 0, 2047, 1023, 8, 160, 165, 172);

    // Title Header Bars (Clean geometric bars)
    img.fillRect(1080, 935, 1260, 955, 30, 45, 90);
    img.fillRect(1280, 937, 1460, 953, 45, 65, 120);
    img.fillRect(1480, 939, 1640, 951, 65, 85, 140);
    img.drawHLine(1040, 2030, 910, 40, 60, 120);

    // Column Header Bars (TODO / IN PROGRESS / DONE)
    img.fillRect(1120, 842, 1260, 866, 200, 40, 40);  // Red (TODO)
    img.fillRect(1440, 842, 1620, 866, 220, 135, 20); // Orange (IN PROGRESS)
    img.fillRect(1780, 842, 1940, 866, 35, 160, 50);  // Green (DONE)

    img.drawVLine(1360, 180, 910, 180, 185, 190);
    img.drawVLine(1700, 180, 910, 180, 185, 190);

    // Helper lambda to draw clean placeholder text bars on sticky notes
    auto drawStickyBars = [&](int px0, int py0, int px1, int py1) {
        int noteMid = (py0 + py1) / 2;
        img.fillRect(px0 + 12, noteMid + 14, px1 - 16, noteMid + 22, 55, 55, 60);
        img.fillRect(px0 + 12, noteMid - 4,  px1 - 32, noteMid + 4,  65, 65, 70);
        img.fillRect(px0 + 12, noteMid - 22, px1 - 48, noteMid - 14, 75, 75, 80);
    };

    // Sticky Notes (Colored cards with neat rectangular text bars)
    // Column 1 (TODO)
    img.fillRect(1080, 680, 1220, 780, 255, 240, 130);
    drawStickyBars(1080, 680, 1220, 780);

    img.fillRect(1230, 680, 1340, 780, 255, 170, 180);
    drawStickyBars(1230, 680, 1340, 780);

    img.fillRect(1080, 550, 1220, 650, 160, 230, 255);
    drawStickyBars(1080, 550, 1220, 650);

    // Column 2 (IN PROGRESS)
    img.fillRect(1390, 680, 1530, 780, 255, 200, 100);
    drawStickyBars(1390, 680, 1530, 780);

    img.fillRect(1540, 680, 1680, 780, 255, 255, 120);
    drawStickyBars(1540, 680, 1680, 780);

    img.fillRect(1390, 550, 1530, 650, 200, 255, 160);
    drawStickyBars(1390, 550, 1530, 650);

    // Column 3 (DONE)
    img.fillRect(1730, 680, 1870, 780, 180, 245, 180);
    drawStickyBars(1730, 680, 1870, 780);

    img.fillRect(1880, 680, 2020, 780, 180, 245, 180);
    drawStickyBars(1880, 680, 2020, 780);

    img.fillRect(1730, 550, 1870, 650, 180, 245, 180);
    drawStickyBars(1730, 550, 1870, 650);

    // Bottom Deadline section
    img.drawHLine(1040, 2030, 160, 200, 50, 50);
    img.fillRect(1080, 85, 1520, 108, 220, 35, 35);
    img.fillRect(1080, 48, 1360, 68, 190, 45, 45);

    // ------------------------------------------------------------------------
    // 3. Panoramic Window City Skyline View [0..1023, 1024..2047]
    // ------------------------------------------------------------------------
    for (int y = 1024; y < 2048; ++y) {
        float t = (float)(y - 1024) / 1024.0f;
        uint8_t skyR = (uint8_t)(135 + t * 90);
        uint8_t skyG = (uint8_t)(195 + t * 45);
        uint8_t skyB = (uint8_t)(245 + t * 10);
        img.drawHLine(0, 1023, y, skyR, skyG, skyB);
    }
    img.fillCircle(240, 1700, 80, 255, 255, 255);
    img.fillCircle(320, 1720, 100, 255, 255, 255);
    img.fillCircle(400, 1700, 70, 255, 255, 255);
    img.fillCircle(760, 1760, 90, 255, 255, 255);
    img.fillCircle(860, 1780, 110, 255, 255, 255);

    int bldHeights[] = { 640, 480, 760, 580, 820, 520, 700, 600, 440 };
    int bldWidths[]  = { 110, 90,  130, 100, 140, 110, 120, 100, 120 };
    int curBx = 20;
    for (int b = 0; b < 9; ++b) {
        int w = bldWidths[b];
        int h = bldHeights[b];
        uint8_t col = 60 + b * 15;
        img.fillRect(curBx, 1024, curBx + w, 1024 + h, col, col + 12, col + 28);
        img.drawRectOutline(curBx, 1024, curBx + w, 1024 + h, 2, 35, 45, 65);
        for (int wy = 1050; wy < 1024 + h - 30; wy += 28) {
            for (int wx = curBx + 12; wx < curBx + w - 12; wx += 20) {
                if ((wx * 13 + wy * 7) % 5 == 0) {
                    img.fillRect(wx, wy, wx + 10, wy + 14, 255, 245, 170);
                } else if ((wx * 11 + wy * 19) % 4 == 0) {
                    img.fillRect(wx, wy, wx + 10, wy + 14, 180, 220, 255);
                }
            }
        }
        curBx += w + 4;
    }

    // ------------------------------------------------------------------------
    // 4. Primary Monitor: VS Code IDE (HD Code Bars) [1024..1279, 1024..1535]
    // ------------------------------------------------------------------------
    int sc1X0 = 1024, sc1Y0 = 1024, sc1X1 = 1279, sc1Y1 = 1535;
    img.fillRect(sc1X0, sc1Y0, sc1X1, sc1Y1, 30, 30, 30);
    // Tab bar at top
    img.fillRect(sc1X0, sc1Y1 - 28, sc1X1, sc1Y1, 45, 45, 48);
    img.fillRect(sc1X0 + 10, sc1Y1 - 26, sc1X0 + 110, sc1Y1 - 4, 30, 30, 30);
    img.fillRect(sc1X0 + 18, sc1Y1 - 18, sc1X0 + 80, sc1Y1 - 12, 100, 200, 255); // Cyan active tab bar
    img.fillRect(sc1X0 + 125, sc1Y1 - 18, sc1X0 + 190, sc1Y1 - 12, 140, 145, 155); // Inactive tab bar

    // Explorer Sidebar
    img.fillRect(sc1X0, sc1Y0 + 20, sc1X0 + 16, sc1Y1 - 28, 51, 51, 51);
    img.fillRect(sc1X0 + 16, sc1Y0 + 20, sc1X0 + 45, sc1Y1 - 28, 37, 37, 38);
    img.fillRect(sc1X0 + 20, sc1Y1 - 46, sc1X0 + 40, sc1Y1 - 40, 160, 170, 185);
    img.fillRect(sc1X0 + 20, sc1Y1 - 58, sc1X0 + 38, sc1Y1 - 52, 130, 140, 150);

    // Syntax-Highlighted Code Lines (Clean rectangular code tokens)
    int codeX = sc1X0 + 52;
    int lineY = sc1Y1 - 48;
    int lineSpacing = 16;
    for (int l = 0; l < 11; ++l) {
        // Line number block
        img.fillRect(codeX, lineY + 2, codeX + 10, lineY + 9, 85, 90, 100);
        int curX = codeX + 16;
        if (l == 0 || l == 1) { // #include <...>
            img.fillRect(curX, lineY + 2, curX + 28, lineY + 9, 197, 134, 192); curX += 34;
            img.fillRect(curX, lineY + 2, curX + 55, lineY + 9, 206, 145, 120);
        } else if (l == 2) { // comment
            img.fillRect(curX, lineY + 2, curX + 75, lineY + 9, 106, 153, 85);
        } else if (l == 3) { // void TrainModel()
            img.fillRect(curX, lineY + 2, curX + 24, lineY + 9, 86, 156, 214); curX += 30;
            img.fillRect(curX, lineY + 2, curX + 50, lineY + 9, 220, 220, 170);
        } else if (l == 4) { // indent + auto ai = ...
            curX += 16;
            img.fillRect(curX, lineY + 2, curX + 22, lineY + 9, 86, 156, 214); curX += 28;
            img.fillRect(curX, lineY + 2, curX + 18, lineY + 9, 156, 220, 254); curX += 24;
            img.fillRect(curX, lineY + 2, curX + 48, lineY + 9, 220, 220, 170);
        } else if (l == 5) { // indent + ai->Run();
            curX += 16;
            img.fillRect(curX, lineY + 2, curX + 20, lineY + 9, 156, 220, 254); curX += 26;
            img.fillRect(curX, lineY + 2, curX + 42, lineY + 9, 220, 220, 170);
        } else if (l == 6) { // indent + float loss = ...
            curX += 16;
            img.fillRect(curX, lineY + 2, curX + 24, lineY + 9, 86, 156, 214); curX += 30;
            img.fillRect(curX, lineY + 2, curX + 22, lineY + 9, 156, 220, 254); curX += 28;
            img.fillRect(curX, lineY + 2, curX + 35, lineY + 9, 181, 206, 168);
        } else if (l == 7) { // indent + if (...)
            curX += 16;
            img.fillRect(curX, lineY + 2, curX + 18, lineY + 9, 197, 134, 192); curX += 24;
            img.fillRect(curX, lineY + 2, curX + 38, lineY + 9, 156, 220, 254);
        } else if (l == 8) { // double indent
            curX += 32;
            img.fillRect(curX, lineY + 2, curX + 45, lineY + 9, 220, 180, 100);
        } else {
            curX += (l % 2 == 0 ? 16 : 0);
            img.fillRect(curX, lineY + 2, curX + 35, lineY + 9, 86, 156, 214); curX += 40;
            img.fillRect(curX, lineY + 2, curX + 40, lineY + 9, 220, 220, 170);
        }
        lineY -= lineSpacing;
    }

    // Terminal area at bottom
    img.fillRect(sc1X0, sc1Y0 + 20, sc1X1, sc1Y0 + 115, 24, 24, 24);
    img.drawHLine(sc1X0, sc1X1, sc1Y0 + 115, 60, 60, 65);
    img.fillRect(sc1X0 + 10, sc1Y0 + 95, sc1X0 + 90, sc1Y0 + 105, 78, 201, 176); // Green terminal tag
    img.fillRect(sc1X0 + 100, sc1Y0 + 95, sc1X0 + 180, sc1Y0 + 105, 100, 255, 100);
    img.fillRect(sc1X0 + 10, sc1Y0 + 75, sc1X0 + 150, sc1Y0 + 85, 220, 220, 220);
    img.fillRect(sc1X0 + 10, sc1Y0 + 55, sc1X0 + 120, sc1Y0 + 65, 100, 200, 255);
    img.fillRect(sc1X0 + 10, sc1Y0 + 35, sc1X0 + 60, sc1Y0 + 45, 255, 255, 255);

    // Status bar at very bottom
    img.fillRect(sc1X0, sc1Y0, sc1X1, sc1Y0 + 18, 104, 33, 122);
    img.fillRect(sc1X0 + 8, sc1Y0 + 5, sc1X0 + 45, sc1Y0 + 13, 240, 240, 250);
    img.fillRect(sc1X0 + 55, sc1Y0 + 5, sc1X0 + 110, sc1Y0 + 13, 200, 200, 210);
    img.fillRect(sc1X1 - 50, sc1Y0 + 5, sc1X1 - 10, sc1Y0 + 13, 220, 220, 240);

    // ------------------------------------------------------------------------
    // 5. Secondary Monitor: Telemetry & Dashboard [1280..1535, 1024..1535]
    // ------------------------------------------------------------------------
    int sc2X0 = 1280, sc2Y0 = 1024, sc2X1 = 1535, sc2Y1 = 1535;
    img.fillRect(sc2X0, sc2Y0, sc2X1, sc2Y1, 18, 22, 32);
    // Header Bar
    img.fillRect(sc2X0, sc2Y1 - 26, sc2X1, sc2Y1, 30, 38, 55);
    img.fillRect(sc2X0 + 12, sc2Y1 - 18, sc2X0 + 120, sc2Y1 - 10, 100, 210, 255);
    img.fillRect(sc2X1 - 60, sc2Y1 - 18, sc2X1 - 12, sc2Y1 - 10, 80, 220, 120);

    // Two Circular Dial Gauges
    int g1X = sc2X0 + 64, g1Y = sc2Y1 - 70;
    img.fillCircle(g1X, g1Y, 34, 30, 36, 50);
    img.drawRing(g1X, g1Y, 26, 34, 255, 80, 80);
    img.fillRect(g1X - 16, g1Y - 4, g1X + 16, g1Y + 4, 255, 255, 255); // Inner value bar

    int g2X = sc2X0 + 180, g2Y = sc2Y1 - 70;
    img.fillCircle(g2X, g2Y, 34, 30, 36, 50);
    img.drawRing(g2X, g2Y, 26, 34, 50, 220, 130);
    img.fillRect(g2X - 16, g2Y - 4, g2X + 16, g2Y + 4, 255, 255, 255); // Inner value bar

    // Loss Curve Graph Area
    img.drawRectOutline(sc2X0 + 12, sc2Y0 + 220, sc2X1 - 12, sc2Y0 + 370, 1, 50, 70, 100);
    img.fillRect(sc2X0 + 20, sc2Y0 + 355, sc2X0 + 110, sc2Y0 + 363, 140, 200, 255); // Graph title bar
    for (int gy = sc2Y0 + 245; gy <= sc2Y0 + 345; gy += 25) {
        img.drawHLine(sc2X0 + 15, sc2X1 - 15, gy, 35, 48, 68);
    }
    for (int px = sc2X0 + 20; px < sc2X1 - 20; ++px) {
        float normX = (float)(px - (sc2X0 + 20)) / (float)(sc2X1 - sc2X0 - 40);
        float normY = std::exp(-normX * 4.0f) * 0.85f + 0.08f;
        int py = (sc2Y0 + 235) + (int)(normY * 105.0f);
        img.setPixel(px, py, 50, 255, 140);
        img.setPixel(px, py + 1, 50, 255, 140);
    }

    // Telemetry System Metrics (Clean multi-colored status bars)
    img.fillRect(sc2X0 + 12, sc2Y0 + 15, sc2X1 - 12, sc2Y0 + 200, 12, 16, 24);
    img.drawRectOutline(sc2X0 + 12, sc2Y0 + 15, sc2X1 - 12, sc2Y0 + 200, 1, 40, 60, 90);
    img.fillRect(sc2X0 + 18, sc2Y0 + 175, sc2X0 + 130, sc2Y0 + 187, 255, 215, 0); // Gold category bar

    int metY = sc2Y0 + 155;
    for (int m = 0; m < 5; ++m) {
        // Status indicator dot
        img.fillRect(sc2X0 + 18, metY + 1, sc2X0 + 24, metY + 7, (m == 3 ? 255 : 80), (m == 3 ? 80 : 220), (m == 3 ? 80 : 120));
        // Metric label bar
        img.fillRect(sc2X0 + 30, metY, sc2X0 + 110, metY + 8, 140, 160, 190);
        // Metric progress bar
        img.fillRect(sc2X0 + 125, metY, sc2X0 + 125 + (m * 19 + 45) % 90, metY + 8, (m == 3 ? 240 : 60), (m == 3 ? 100 : 180), (m == 3 ? 80 : 255));
        metY -= 25;
    }

    // ------------------------------------------------------------------------
    // 6. PC Case Front Mesh with 3 Glowing RGB Fans [1536..1791, 1024..1279]
    // ------------------------------------------------------------------------
    int pcfX0 = 1536, pcfY0 = 1024, pcfX1 = 1791, pcfY1 = 1279;
    img.fillRect(pcfX0, pcfY0, pcfX1, pcfY1, 22, 22, 24);
    for (int y = pcfY0; y <= pcfY1; y += 8) {
        for (int x = pcfX0; x <= pcfX1; x += 8) {
            img.setPixel(x + ((y / 8) % 2 ? 4 : 0), y, 38, 38, 42);
        }
    }
    int fanCentersY[] = { pcfY0 + 45, pcfY0 + 128, pcfY0 + 210 };
    int fanCenterX = (pcfX0 + pcfX1) / 2;
    for (int f = 0; f < 3; ++f) {
        int cy = fanCentersY[f];
        img.drawRing(fanCenterX, cy, 32, 38, 0, 230, 255);
        img.drawRing(fanCenterX, cy, 28, 32, 255, 0, 180);
        img.fillCircle(fanCenterX, cy, 12, 45, 45, 50);
        img.drawRing(fanCenterX, cy, 12, 14, 255, 255, 255);
    }

    // ------------------------------------------------------------------------
    // 7. PC Case Tempered Glass Side Window [1792..2047, 1024..1279]
    // ------------------------------------------------------------------------
    int pcsX0 = 1792, pcsY0 = 1024, pcsX1 = 2047, pcsY1 = 1279;
    img.fillRect(pcsX0, pcsY0, pcsX1, pcsY1, 15, 17, 22);
    img.drawRectOutline(pcsX0, pcsY0, pcsX1, pcsY1, 6, 40, 42, 48);

    img.fillRect(pcsX0 + 20, pcsY0 + 20, pcsX1 - 20, pcsY1 - 20, 28, 30, 36);
    img.fillRect(pcsX0 + 35, pcsY0 + 140, pcsX0 + 75, pcsY0 + 200, 140, 145, 155);

    int cpuX = pcsX0 + 105, cpuY = pcsY0 + 160;
    img.fillCircle(cpuX, cpuY, 26, 10, 10, 15);
    img.drawRing(cpuX, cpuY, 20, 26, 0, 240, 255);
    img.drawRing(cpuX, cpuY, 14, 18, 255, 0, 150);
    img.drawString(cpuX - 14, cpuY - 4, "65C", 1, 255, 255, 255);

    int ramX = pcsX0 + 150, ramY = pcsY0 + 130;
    img.fillRect(ramX, ramY, ramX + 8, ramY + 55, 255, 100, 200);
    img.fillRect(ramX + 12, ramY, ramX + 20, ramY + 55, 100, 200, 255);

    int gpuX0 = pcsX0 + 25, gpuY0 = pcsY0 + 40, gpuX1 = pcsX1 - 30, gpuY1 = pcsY0 + 110;
    img.fillRect(gpuX0, gpuY0, gpuX1, gpuY1, 35, 38, 44);
    img.drawRectOutline(gpuX0, gpuY0, gpuX1, gpuY1, 2, 80, 85, 95);
    img.fillRect(gpuX0 + 15, gpuY1 - 20, gpuX0 + 120, gpuY1 - 6, 10, 10, 12);
    img.drawString(gpuX0 + 20, gpuY1 - 16, "GEFORCE RTX 4090", 1, 118, 185, 0);

    int gpuFanCenters[] = { gpuX0 + 45, gpuX0 + 100, gpuX0 + 155 };
    for (int gx : gpuFanCenters) {
        img.fillCircle(gx, gpuY0 + 35, 20, 20, 22, 26);
        img.drawRing(gx, gpuY0 + 35, 16, 20, 120, 130, 140);
    }

    // ------------------------------------------------------------------------
    // 8. Custom Mechanical Keyboard [1536..1791, 1280..1535]
    // ------------------------------------------------------------------------
    int kbX0 = 1536, kbY0 = 1280, kbX1 = 1791, kbY1 = 1535;
    img.fillRect(kbX0, kbY0, kbX1, kbY1, 35, 37, 42);
    int rowY = kbY1 - 25;
    for (int r = 0; r < 5; ++r) {
        int curKx = kbX0 + 15;
        for (int k = 0; k < 14; ++k) {
            uint8_t kr = 220, kg = 220, kb = 225;
            if (r == 0 || k == 0 || k >= 12) {
                kr = 70; kg = 75; kb = 85;
            }
            if (r == 0 && k == 0) { kr = 255; kg = 90; kb = 30; } // Orange ESC
            if (r == 2 && k == 12) { kr = 0; kg = 180; kb = 230; } // Cyan Enter
            img.fillRect(curKx, rowY - 12, curKx + 13, rowY, kr, kg, kb);
            img.drawRectOutline(curKx, rowY - 12, curKx + 13, rowY, 1, 25, 25, 30);
            curKx += 16;
        }
        rowY -= 15;
    }
    img.fillRect(kbX0 + 80, kbY1 - 100, kbX0 + 165, kbY1 - 87, 220, 220, 225);

    // ------------------------------------------------------------------------
    // 9. Studio Monitor Speakers [1792..2047, 1280..1535]
    // ------------------------------------------------------------------------
    int spkX0 = 1792, spkY0 = 1280, spkX1 = 2047, spkY1 = 1535;
    img.fillRect(spkX0, spkY0, spkX1, spkY1, 32, 34, 38);
    img.drawRectOutline(spkX0, spkY0, spkX1, spkY1, 4, 18, 18, 22);
    int spkMidX = (spkX0 + spkX1) / 2;
    img.fillCircle(spkMidX, spkY1 - 60, 24, 20, 22, 26);
    img.fillCircle(spkMidX, spkY1 - 60, 12, 180, 185, 195);
    img.fillCircle(spkMidX, spkY0 + 80, 56, 225, 190, 50);
    img.drawRing(spkMidX, spkY0 + 80, 44, 56, 25, 25, 30);
    img.fillCircle(spkMidX, spkY0 + 80, 18, 20, 20, 25);

    // ------------------------------------------------------------------------
    // 10. Clean Office Door (NO PLAQUE!) [1024..1279, 1536..2047]
    // ------------------------------------------------------------------------
    img.fillRect(1024, 1536, 1279, 2047, 140, 100, 70);
    img.drawRectOutline(1024, 1536, 1279, 2047, 8, 90, 60, 40);
    img.drawRectOutline(1048, 1560, 1255, 1680, 6, 100, 70, 45);
    img.drawRectOutline(1048, 1710, 1255, 1840, 6, 100, 70, 45);
    img.drawRectOutline(1048, 1870, 1255, 2010, 6, 100, 70, 45);
    // Door plaque removed as requested!

    // Clock Face [1280..1407, 1536..1663]
    int clkX = 1344, clkY = 1600;
    // Clean background
    img.fillRect(1280, 1536, 1407, 1663, 235, 238, 242);
    // Crisp white dial
    img.fillCircle(clkX, clkY, 60, 255, 255, 255);

    // 12 Hour Tick Marks around the dial
    for (int h = 0; h < 12; ++h) {
        float ang = (float)h * 3.14159265f / 6.0f;
        float ca = std::cos(ang), sa = std::sin(ang);
        int rOut = 55;
        int rIn = (h % 3 == 0) ? 44 : 49; // 12, 3, 6, 9 are bold major ticks
        int thick = (h % 3 == 0) ? 3 : 2;

        int xA = clkX + (int)(rIn * ca);
        int yA = clkY + (int)(rIn * sa);
        int xB = clkX + (int)(rOut * ca);
        int yB = clkY + (int)(rOut * sa);

        int minTx = std::min(xA, xB), maxTx = std::max(xA, xB);
        int minTy = std::min(yA, yB), maxTy = std::max(yA, yB);
        if (h == 0 || h == 6) { // Horizontal ticks (3 and 9 o'clock)
            img.fillRect(minTx, clkY - thick/2, maxTx, clkY + thick/2, 25, 25, 30);
        } else if (h == 3 || h == 9) { // Vertical ticks (12 and 6 o'clock)
            img.fillRect(clkX - thick/2, minTy, clkX + thick/2, maxTy, 25, 25, 30);
        } else { // Diagonal ticks
            img.fillRect(minTx - 1, minTy - 1, maxTx + 1, maxTy + 1, 35, 35, 40);
        }
    }

    // Hour Hand (pointing towards 10 o'clock, pivoting from exact center)
    for (int t = 0; t <= 24; ++t) {
        float f = (float)t / 24.0f;
        int hx = clkX + (int)(-20.0f * f);
        int hy = clkY + (int)(15.0f * f);
        img.fillCircle(hx, hy, 2, 20, 20, 25);
    }

    // Minute Hand (pointing towards 2 o'clock, pivoting from exact center)
    for (int t = 0; t <= 40; ++t) {
        float f = (float)t / 40.0f;
        int mx = clkX + (int)(20.0f * f);
        int my = clkY + (int)(34.0f * f);
        img.fillCircle(mx, my, 2, 20, 20, 25);
    }

    // Second Hand (vibrant red line through center down to 6 o'clock)
    for (int sy = clkY - 48; sy <= clkY + 10; ++sy) {
        img.setPixel(clkX, sy, 220, 30, 30);
        img.setPixel(clkX + 1, sy, 220, 30, 30);
    }

    // Center Cap
    img.fillCircle(clkX, clkY, 4, 30, 30, 35);
    img.fillCircle(clkX, clkY, 2, 220, 30, 30);

    // Vision Poster [1280..1535, 1664..2047]
    // Clean stylized graphic rectangle bars ("ขีดสี่เหลี่ยม") as requested!
    img.fillRect(1280, 1664, 1535, 2047, 20, 25, 35);
    img.drawRectOutline(1280, 1664, 1535, 2047, 12, 210, 180, 90);

    // Title Bars (Gold)
    img.fillRect(1310, 1968, 1505, 1988, 255, 215, 0);
    img.fillRect(1335, 1942, 1480, 1956, 240, 195, 10);

    // Content Bars (White / Silver)
    img.fillRect(1325, 1905, 1490, 1918, 235, 238, 245);
    img.fillRect(1350, 1878, 1465, 1891, 210, 215, 225);
    img.fillRect(1335, 1852, 1480, 1865, 235, 238, 245);

    // Highlight Bars (Cyan)
    img.fillRect(1315, 1805, 1500, 1823, 70, 200, 255);
    img.fillRect(1340, 1780, 1475, 1794, 50, 170, 230);

    // Footer Bar (Slate)
    img.fillRect(1355, 1715, 1460, 1726, 130, 140, 160);

    // Bookshelf Spines & Binders [1536..1791, 1536..1791]
    img.fillRect(1536, 1536, 1791, 1791, 45, 30, 20);
    int curBx2 = 1540;
    struct BookInfo { const char* title; uint8_t r, g, b; int w; };
    BookInfo books[] = {
        {"OPENGL", 180, 40, 40, 24},
        {"C++20", 30, 80, 180, 22},
        {"ALGO", 40, 150, 60, 20},
        {"AI LAB", 160, 130, 30, 24},
        {"SHADERS", 140, 40, 140, 22},
        {"MATH 3D", 50, 120, 150, 20},
        {"SYSTEMS", 180, 90, 30, 24},
        {"BINDER 1", 210, 210, 210, 28},
        {"BINDER 2", 210, 210, 210, 28},
        {"BINDER 3", 50, 50, 50, 28},
    };
    for (const auto& b : books) {
        img.fillRect(curBx2, 1550, curBx2 + b.w - 2, 1780, b.r, b.g, b.b);
        img.drawRectOutline(curBx2, 1550, curBx2 + b.w - 2, 1780, 1, 20, 20, 20);
        img.fillRect(curBx2 + 2, 1560, curBx2 + b.w - 4, 1566, 255, 215, 0);
        img.fillRect(curBx2 + 2, 1764, curBx2 + b.w - 4, 1770, 255, 215, 0);
        curBx2 += b.w;
    }

    // Desk Wood [1792..2047, 1536..1791]
    img.fillRect(1792, 1536, 2047, 1791, 75, 45, 28);
    for (int y = 1536; y < 1792; y += 4) {
        uint8_t grain = 70 + (y % 15);
        img.drawHLine(1792, 2047, y, grain, (uint8_t)(grain * 0.62f), (uint8_t)(grain * 0.40f));
    }

    // Desk Mat [1536..1791, 1792..1919]
    img.fillRect(1536, 1792, 1791, 1919, 28, 30, 36);
    img.drawRectOutline(1536, 1792, 1791, 1919, 4, 0, 210, 255);

    // Aluminum Trim [1792..2047, 1792..1855]
    img.fillRect(1792, 1792, 2047, 1855, 210, 215, 222);

    // Plant Foliage [1792..1919, 1856..1983]
    img.fillRect(1792, 1856, 1919, 1983, 35, 115, 45);
    for (int y = 1856; y < 1984; y += 8) {
        img.drawHLine(1792, 1919, y, 50, 145, 60);
    }

    // Drywall [1920..2047, 1856..1983]
    img.fillRect(1920, 1856, 2047, 1983, 235, 238, 242);

    // Ceiling Tiles [1536..1791, 1920..2047]
    img.fillRect(1536, 1920, 1791, 2047, 240, 242, 245);
    img.drawRectOutline(1536, 1920, 1791, 2047, 4, 205, 210, 215);

    // Metallic Steel [1792..2047, 1984..2047]
    img.fillRect(1792, 1984, 2047, 2047, 190, 195, 202);

    img.saveBMP("Textures/scene_room.bmp");
    std::cout << "[generate_office] Saved Textures/scene_room.bmp (2048x2048 HD)\n";

    // ========================================================================
    // Procedural 3D Office Room Geometry (.OBJ)
    // ========================================================================
    OBJBuilder obj;

    UVBox uvFloor       = { 0.0f,     0.0f,     0.5f,     0.5f };
    UVBox uvWhiteboard  = { 0.5f,     0.0f,     1.0f,     0.5f };
    UVBox uvWindow      = { 0.0f,     0.5f,     0.5f,     1.0f };
    UVBox uvScreenMain  = { 0.50f,    0.50f,    0.625f,   0.75f };
    UVBox uvScreenSec   = { 0.625f,   0.50f,    0.75f,    0.75f };
    UVBox uvPCFront     = { 0.75f,    0.50f,    0.875f,   0.625f };
    UVBox uvPCSide      = { 0.875f,   0.50f,    1.0f,     0.625f };
    UVBox uvKeyboard    = { 0.75f,    0.625f,   0.875f,   0.75f };
    UVBox uvSpeaker     = { 0.875f,   0.625f,   1.0f,     0.75f };
    UVBox uvDoor        = { 0.50f,    0.75f,    0.625f,   1.0f };
    UVBox uvClock       = { 0.625f,   0.75f,    0.6875f,  0.8125f };
    UVBox uvPoster      = { 0.625f,   0.8125f,  0.75f,    1.0f };
    UVBox uvBooks       = { 0.75f,    0.75f,    0.875f,   0.875f };
    UVBox uvDeskWood    = { 0.875f,   0.75f,    1.0f,     0.875f };
    UVBox uvDeskMat     = { 0.75f,    0.875f,   0.875f,   0.9375f };
    UVBox uvAlumTrim    = { 0.875f,   0.875f,   1.0f,     0.90625f };
    UVBox uvFoliage     = { 0.875f,   0.90625f, 0.9375f,  0.96875f };
    UVBox uvDrywall     = { 0.9375f,  0.90625f, 1.0f,     0.96875f };
    UVBox uvCeiling     = { 0.75f,    0.9375f,  0.875f,   1.0f };
    UVBox uvSteel       = { 0.875f,   0.96875f, 1.0f,     1.0f };

    // ------------------------------------------------------------------------
    // 1. SEAMLESS BROWN WOOD FLOOR (Spanning Entire Room)
    // ------------------------------------------------------------------------
    for (int iz = 0; iz < 7; ++iz) {
        float z0 = -5.0f + iz * 1.0f;
        float z1 = z0 + 1.0f;
        for (int ix = 0; ix < 7; ++ix) {
            float x0 = -2.5f + ix * 1.0f;
            float x1 = x0 + 1.0f;

            obj.addQuad(Vec3(x0, -1.50f, z1), Vec3(x1, -1.50f, z1),
                        Vec3(x1, -1.50f, z0), Vec3(x0, -1.50f, z0),
                        Vec2(uvFloor.u0, uvFloor.v0), Vec2(uvFloor.u1, uvFloor.v0),
                        Vec2(uvFloor.u1, uvFloor.v1), Vec2(uvFloor.u0, uvFloor.v1),
                        Vec3(0, 1.0f, 0));
        }
    }

    // Ceiling (Y = 1.70f)
    obj.addQuad(Vec3(-2.5f, 1.70f, -5.0f), Vec3( 4.5f, 1.70f, -5.0f),
                Vec3( 4.5f, 1.70f,  2.0f), Vec3(-2.5f, 1.70f,  2.0f),
                Vec2(uvCeiling.u0, uvCeiling.v0), Vec2(uvCeiling.u1, uvCeiling.v0),
                Vec2(uvCeiling.u1, uvCeiling.v1), Vec2(uvCeiling.u0, uvCeiling.v1),
                Vec3(0, -1.0f, 0));

    // Back Wall (Z = -5.0f)
    obj.addQuad(Vec3(-2.5f, -1.50f, -5.0f), Vec3( 4.5f, -1.50f, -5.0f),
                Vec3( 4.5f,  1.70f, -5.0f), Vec3(-2.5f,  1.70f, -5.0f),
                Vec2(uvDrywall.u0, uvDrywall.v0), Vec2(uvDrywall.u1, uvDrywall.v0),
                Vec2(uvDrywall.u1, uvDrywall.v1), Vec2(uvDrywall.u0, uvDrywall.v1),
                Vec3(0, 0, 1.0f));

    // Left Wall (X = -2.5f)
    obj.addQuad(Vec3(-2.5f, -1.50f,  2.0f), Vec3(-2.5f, -1.50f, -5.0f),
                Vec3(-2.5f,  1.70f, -5.0f), Vec3(-2.5f,  1.70f,  2.0f),
                Vec2(uvDrywall.u0, uvDrywall.v0), Vec2(uvDrywall.u1, uvDrywall.v0),
                Vec2(uvDrywall.u1, uvDrywall.v1), Vec2(uvDrywall.u0, uvDrywall.v1),
                Vec3(1.0f, 0, 0));

    // Right Wall (X = 4.5f)
    obj.addQuad(Vec3( 4.5f, -1.50f, -5.0f), Vec3( 4.5f, -1.50f,  2.0f),
                Vec3( 4.5f,  1.70f,  2.0f), Vec3( 4.5f,  1.70f, -5.0f),
                Vec2(uvDrywall.u0, uvDrywall.v0), Vec2(uvDrywall.u1, uvDrywall.v0),
                Vec2(uvDrywall.u1, uvDrywall.v1), Vec2(uvDrywall.u0, uvDrywall.v1),
                Vec3(-1.0f, 0, 0));

    // Baseboard moldings
    obj.addBox(-2.48f, -1.50f, -4.98f,  4.48f, -1.42f, -4.95f, uvDeskWood);
    obj.addBox(-2.48f, -1.50f, -4.98f, -2.45f, -1.42f,  1.98f, uvDeskWood);
    obj.addBox( 4.45f, -1.50f, -4.98f,  4.48f, -1.42f,  1.98f, uvDeskWood);

    // ------------------------------------------------------------------------
    // 2. Panoramic Window (Left Wall)
    // ------------------------------------------------------------------------
    obj.addBox(-2.47f,  1.25f, -4.20f, -2.43f,  1.30f, -0.60f, uvSteel);
    obj.addBox(-2.47f, -0.62f, -4.25f, -2.40f, -0.58f, -0.55f, uvSteel);
    obj.addBox(-2.47f, -0.58f, -4.20f, -2.43f,  1.25f, -4.15f, uvSteel);
    obj.addBox(-2.47f, -0.58f, -0.65f, -2.43f,  1.25f, -0.60f, uvSteel);
    obj.addBox(-2.47f, -0.58f, -2.42f, -2.43f,  1.25f, -2.38f, uvSteel);
    obj.addBox(-2.47f,  0.35f, -4.15f, -2.43f,  0.40f, -0.65f, uvSteel);
    obj.addQuad(Vec3(-2.48f, -0.58f, -0.65f), Vec3(-2.48f, -0.58f, -4.15f),
                Vec3(-2.48f,  1.25f, -4.15f), Vec3(-2.48f,  1.25f, -0.65f),
                Vec2(uvWindow.u0, uvWindow.v0), Vec2(uvWindow.u1, uvWindow.v0),
                Vec2(uvWindow.u1, uvWindow.v1), Vec2(uvWindow.u0, uvWindow.v1),
                Vec3(1.0f, 0, 0));

    // ------------------------------------------------------------------------
    // 3. Office Door (Right Wall) - Clean Wooden Door (NO PLAQUE!)
    // ------------------------------------------------------------------------
    obj.addBox(4.44f, -1.50f, -0.65f, 4.47f,  1.15f, -0.60f, uvDeskWood);
    obj.addBox(4.44f, -1.50f,  0.60f, 4.47f,  1.15f,  0.65f, uvDeskWood);
    obj.addBox(4.44f,  1.10f, -0.65f, 4.47f,  1.15f,  0.65f, uvDeskWood);
    obj.addQuad(Vec3(4.46f, -1.50f, -0.60f), Vec3(4.46f, -1.50f,  0.60f),
                Vec3(4.46f,  1.10f,  0.60f), Vec3(4.46f,  1.10f, -0.60f),
                Vec2(uvDoor.u0, uvDoor.v0), Vec2(uvDoor.u1, uvDoor.v0),
                Vec2(uvDoor.u1, uvDoor.v1), Vec2(uvDoor.u0, uvDoor.v1),
                Vec3(-1.0f, 0, 0));
    obj.addBox(4.42f, -0.30f, 0.42f, 4.46f, -0.26f, 0.54f, uvSteel);

    // ------------------------------------------------------------------------
    // 4. Agile Scrum Whiteboard (Back Wall)
    // ------------------------------------------------------------------------
    float wbX0 = -0.50f, wbX1 = 2.20f;
    float wbY0 = -0.20f, wbY1 = 1.15f;
    float wbZ  = -4.92f;
    float frW  = 0.04f;

    obj.addBox(wbX0 - frW, wbY1,       -4.96f, wbX1 + frW, wbY1 + frW, -4.90f, uvAlumTrim);
    obj.addBox(wbX0 - frW, wbY0 - frW, -4.96f, wbX1 + frW, wbY0,       -4.90f, uvAlumTrim);
    obj.addBox(wbX0 - frW, wbY0,       -4.96f, wbX0,       wbY1,       -4.90f, uvAlumTrim);
    obj.addBox(wbX1,       wbY0,       -4.96f, wbX1 + frW, wbY1,       -4.90f, uvAlumTrim);
    obj.addBox(wbX0 - frW, wbY0 - 0.07f, -4.96f, wbX1 + frW, wbY0 - frW, -4.84f, uvSteel);

    obj.addQuad(Vec3(wbX0, wbY0, wbZ), Vec3(wbX1, wbY0, wbZ),
                Vec3(wbX1, wbY1, wbZ), Vec3(wbX0, wbY1, wbZ),
                Vec2(uvWhiteboard.u0, uvWhiteboard.v0), Vec2(uvWhiteboard.u1, uvWhiteboard.v0),
                Vec2(uvWhiteboard.u1, uvWhiteboard.v1), Vec2(uvWhiteboard.u0, uvWhiteboard.v1),
                Vec3(0, 0, 1.0f));

    // ------------------------------------------------------------------------
    // 5. Inspirational Vision Poster
    // ------------------------------------------------------------------------
    float pstX0 = 2.70f, pstX1 = 3.80f;
    float pstY0 = 0.28f, pstY1 = 1.22f;
    float pstZ  = -4.92f;

    obj.addBox(pstX0 - frW, pstY1,        -4.96f, pstX1 + frW, pstY1 + frW, -4.90f, uvSteel);
    obj.addBox(pstX0 - frW, pstY0 - frW,  -4.96f, pstX1 + frW, pstY0,       -4.90f, uvSteel);
    obj.addBox(pstX0 - frW, pstY0,        -4.96f, pstX0,       pstY1,       -4.90f, uvSteel);
    obj.addBox(pstX1,       pstY0,        -4.96f, pstX1 + frW, pstY1,       -4.90f, uvSteel);

    obj.addQuad(Vec3(pstX0, pstY0, pstZ), Vec3(pstX1, pstY0, pstZ),
                Vec3(pstX1, pstY1, pstZ), Vec3(pstX0, pstY1, pstZ),
                Vec2(uvPoster.u0, uvPoster.v0), Vec2(uvPoster.u1, uvPoster.v0),
                Vec2(uvPoster.u1, uvPoster.v1), Vec2(uvPoster.u0, uvPoster.v1),
                Vec3(0, 0, 1.0f));

    // ------------------------------------------------------------------------
    // 6. Wall Clock (True Circular Wall Clock with Metallic Bezel)
    // Elevated above the whiteboard (Y = 1.45f) so it never clips or overlaps!
    // ------------------------------------------------------------------------
    float clkCenterX = 0.85f;
    float clkCenterY = 1.45f; // Raised to 1.45f (midpoint between whiteboard at 1.19f and ceiling at 1.70f)
    float clkZWall   = -4.96f;
    float clkZFront  = -4.90f;
    float clkOuterR  = 0.16f;
    float clkFaceR   = 0.144f;
    int   clkSegs    = 32;

    float uMid = (uvClock.u0 + uvClock.u1) * 0.5f;
    float vMid = (uvClock.v0 + uvClock.v1) * 0.5f;
    float uRad = (uvClock.u1 - uvClock.u0) * 0.46875f; // exact 60px / 128px radius
    float vRad = (uvClock.v1 - uvClock.v0) * 0.46875f;

    // Center vertex of the circular clock face
    int ivFaceCenter = obj.addVertex(Vec3(clkCenterX, clkCenterY, clkZFront + 0.002f));
    int itFaceCenter = obj.addUV(Vec2(uMid, vMid));
    int inFace       = obj.addNormal(Vec3(0, 0, 1.0f));

    for (int i = 0; i < clkSegs; ++i) {
        float a0 = (float)i / clkSegs * 2.0f * 3.14159265f;
        float a1 = (float)(i + 1) / clkSegs * 2.0f * 3.14159265f;

        float c0 = std::cos(a0), s0 = std::sin(a0);
        float c1 = std::cos(a1), s1 = std::sin(a1);

        // Outer rim vertices (cylindrical casing extruded along Z from the wall)
        Vec3 r0_back(clkCenterX + clkOuterR * c0, clkCenterY + clkOuterR * s0, clkZWall);
        Vec3 r1_back(clkCenterX + clkOuterR * c1, clkCenterY + clkOuterR * s1, clkZWall);
        Vec3 r1_front(clkCenterX + clkOuterR * c1, clkCenterY + clkOuterR * s1, clkZFront);
        Vec3 r0_front(clkCenterX + clkOuterR * c0, clkCenterY + clkOuterR * s0, clkZFront);

        Vec3 nMid((c0 + c1) * 0.5f, (s0 + s1) * 0.5f, 0.0f);
        obj.addQuad(r0_back, r1_back, r1_front, r0_front,
                    Vec2(uvSteel.u0, uvSteel.v0), Vec2(uvSteel.u1, uvSteel.v0),
                    Vec2(uvSteel.u1, uvSteel.v1), Vec2(uvSteel.u0, uvSteel.v1),
                    nMid);

        // Front beveled rim ring (from outer radius to face radius)
        Vec3 f0_rim(clkCenterX + clkFaceR * c0, clkCenterY + clkFaceR * s0, clkZFront + 0.001f);
        Vec3 f1_rim(clkCenterX + clkFaceR * c1, clkCenterY + clkFaceR * s1, clkZFront + 0.001f);
        obj.addQuad(f0_rim, f1_rim, r1_front, r0_front,
                    Vec2(uvSteel.u0, uvSteel.v0), Vec2(uvSteel.u1, uvSteel.v0),
                    Vec2(uvSteel.u1, uvSteel.v1), Vec2(uvSteel.u0, uvSteel.v1),
                    Vec3(0, 0, 1.0f));

        // Circular Clock Face (True circle triangle fan)
        int ivF0 = obj.addVertex(f0_rim);
        int ivF1 = obj.addVertex(f1_rim);
        int itF0 = obj.addUV(Vec2(uMid + uRad * c0, vMid + vRad * s0));
        int itF1 = obj.addUV(Vec2(uMid + uRad * c1, vMid + vRad * s1));

        obj.faces.push_back({ {ivFaceCenter, itFaceCenter, inFace},
                              {ivF0, itF0, inFace},
                              {ivF1, itF1, inFace} });
    }

    // ------------------------------------------------------------------------
    // 7. Executive Workstation Desk (Right Side) - CLEAN (NO COMPUTER HERE!)
    // As requested: "คอมตรงนี้เอาออก"
    // ------------------------------------------------------------------------
    // Tabletop
    obj.addBox(2.6f, -0.78f, -4.3f, 4.25f, -0.72f, -3.4f, uvDeskWood);
    // Metal Legs
    obj.addBox(2.64f, -1.50f, -4.26f, 2.70f, -0.78f, -4.20f, uvSteel);
    obj.addBox(2.64f, -1.50f, -3.50f, 2.70f, -0.78f, -3.44f, uvSteel);
    // 3-Drawer Unit on Right
    obj.addBox(3.60f, -1.50f, -4.25f, 4.20f, -0.78f, -3.45f, uvDeskWood);
    obj.addBox(3.56f, -0.95f, -3.88f, 3.60f, -0.92f, -3.78f, uvSteel);
    obj.addBox(3.56f, -1.15f, -3.88f, 3.60f, -1.12f, -3.78f, uvSteel);
    obj.addBox(3.56f, -1.35f, -3.88f, 3.60f, -1.32f, -3.78f, uvSteel);

    // Office Desk Accessories for Manager (Lamp, Mug, Pen holder, Document folders)
    // Classic Desk Lamp on Right
    obj.addCylinder(4.05f, -4.05f, -0.72f, -0.705f, 0.08f, 10, uvSteel); // Base
    obj.addCylinder(4.02f, -4.00f, -0.705f, -0.36f, 0.012f, 8, uvSteel); // Arm
    obj.addBox(3.88f, -0.42f, -4.06f, 4.02f, -0.34f, -3.94f, uvSteel); // Shade

    // Coffee Mug (#1 Boss)
    obj.addCylinder(2.82f, -3.65f, -0.72f, -0.62f, 0.045f, 12, uvDrywall);
    // Pen Holder
    obj.addCylinder(4.10f, -3.65f, -0.72f, -0.60f, 0.038f, 10, uvSteel);
    // Document Stacks / Binders on Manager's Desk
    obj.addBox(3.10f, -0.72f, -3.95f, 3.45f, -0.67f, -3.65f, uvBooks);
    obj.addBox(3.55f, -0.72f, -3.90f, 3.85f, -0.70f, -3.68f, uvDrywall);

    // Executive Office Chair (Behind the Desk)
    float chairX = 3.35f, chairZ = -4.55f;
    obj.addCylinder(chairX, chairZ, -1.50f, -1.45f, 0.32f, 8, uvSteel);
    obj.addCylinder(chairX, chairZ, -1.45f, -1.05f, 0.035f, 8, uvSteel);
    obj.addBox(chairX - 0.26f, -1.05f, chairZ - 0.26f, chairX + 0.26f, -0.97f, chairZ + 0.26f, uvDeskMat);
    obj.addBox(chairX - 0.24f, -0.97f, chairZ - 0.28f, chairX + 0.24f, -0.35f, chairZ - 0.22f, uvDeskMat);
    obj.addBox(chairX - 0.16f, -0.35f, chairZ - 0.27f, chairX + 0.16f, -0.22f, chairZ - 0.23f, uvDeskMat);
    obj.addBox(chairX - 0.29f, -0.90f, chairZ - 0.15f, chairX - 0.25f, -0.72f, chairZ + 0.15f, uvSteel);
    obj.addBox(chairX + 0.25f, -0.90f, chairZ - 0.15f, chairX + 0.29f, -0.72f, chairZ + 0.15f, uvSteel);

    // ------------------------------------------------------------------------
    // 8. BATTLESTATION COMPUTERS FOR THE 3 AI WORKERS (Image 3)
    // Deepen AI Workers Workstation Desk (Prevents computers from floating!)
    // As requested: "คอมลอยอยู่ในภาพที่ 1 ทำให้โต๊ะมันกว้างกว่านี้ ไม่ก็ขยับทำแหน่งคอม"
    // Console runs from X = -2.50f to +2.50f
    // Deepen desk in Z from -2.25f to -1.48f (0.77m total depth!)
    // ------------------------------------------------------------------------
    float dX0 = -2.50f, dX1 = 2.50f;
    float dZ0 = -2.25f, dZ1 = -1.48f;
    float dYTop = -0.859f, dYBot = -0.92f;

    // Solid Desktop Slab (matching console desk)
    obj.addBox(dX0, dYBot, dZ0, dX1, dYTop, dZ1, uvDeskWood);
    // Beveled Front Metal Edge Rail
    obj.addBox(dX0, dYBot, dZ1 - 0.03f, dX1, dYTop + 0.005f, dZ1, uvSteel);

    // Sturdy Metallic Desk Legs down to floor (Y = -1.50f)
    float legXs[] = { -2.40f, -0.80f, 0.80f, 2.40f };
    for (float lx : legXs) {
        obj.addBox(lx - 0.04f, -1.50f, dZ1 - 0.08f, lx + 0.04f, dYBot, dZ1 - 0.02f, uvSteel);
        // Base footplate
        obj.addBox(lx - 0.06f, -1.50f, dZ1 - 0.16f, lx + 0.06f, -1.46f, dZ1 + 0.02f, uvSteel);
    }

    // Helper lambda to place a modern developer monitor + stand + keyboard + mouse
    // Now positioned completely on the deepened desk, facing AI workers, with clean back (no black lines)!
    auto addAIWorkstation = [&](float cx, UVBox screenUV, bool isCenter) {
        float bW = 0.31f; // Half width (0.62m total)
        float bH0 = -0.72f, bH1 = -0.38f;
        // Position monitor centered on the deep desk
        float bZ0 = -1.82f, bZ1 = -1.785f;

        // --- Stand & Mounting (Riser pole & base placed behind the monitor on the +Z side) ---
        // Stand base ends at Z = -1.685f (which is 20cm INSIDE the desk front edge at -1.48f!)
        obj.addBox(cx - 0.11f, -0.859f, bZ1, cx + 0.11f, -0.850f, bZ1 + 0.10f, uvSteel); // Stand base
        obj.addCylinder(cx, bZ1 + 0.03f, -0.850f, -0.55f, 0.015f, 8, uvSteel); // Riser pole behind monitor
        // VESA Mount bracket connecting pole to monitor rear
        obj.addBox(cx - 0.04f, -0.62f, bZ1, cx + 0.04f, -0.54f, bZ1 + 0.03f, uvSteel);

        // --- Monitor Bezel Frame (Clean solid steel box - NO black lines or stray quads on back!) ---
        obj.addBox(cx - bW, bH0, bZ0, cx + bW, bH1, bZ1, uvSteel);

        // High-Res Screen Quad (Turned around to face -Z directly towards the AI Worker!)
        obj.addQuad(Vec3(cx + bW - 0.01f, bH0 + 0.01f, bZ0 - 0.002f),
                    Vec3(cx - bW + 0.01f, bH0 + 0.01f, bZ0 - 0.002f),
                    Vec3(cx - bW + 0.01f, bH1 - 0.01f, bZ0 - 0.002f),
                    Vec3(cx + bW - 0.01f, bH1 - 0.01f, bZ0 - 0.002f),
                    Vec2(screenUV.u1, screenUV.v0), Vec2(screenUV.u0, screenUV.v0),
                    Vec2(screenUV.u0, screenUV.v1), Vec2(screenUV.u1, screenUV.v1),
                    Vec3(0, 0, -1.0f));

        // Screen Top Accessory: Webcam on Center, Screenbar on Left/Right (both facing -Z towards worker)
        if (isCenter) {
            obj.addBox(cx - 0.035f, bH1, bZ0 - 0.015f, cx + 0.035f, bH1 + 0.030f, bZ0 + 0.015f, uvSteel);
        } else {
            obj.addBox(cx - 0.16f, bH1 + 0.012f, bZ0 - 0.020f, cx + 0.16f, bH1 + 0.025f, bZ0 + 0.005f, uvSteel);
        }

        // --- Desk Mat ---
        obj.addQuad(Vec3(cx - 0.28f, -0.858f, -1.86f), Vec3(cx + 0.28f, -0.858f, -1.86f),
                    Vec3(cx + 0.28f, -0.858f, -2.18f), Vec3(cx - 0.28f, -0.858f, -2.18f),
                    Vec2(uvDeskMat.u0, uvDeskMat.v0), Vec2(uvDeskMat.u1, uvDeskMat.v0),
                    Vec2(uvDeskMat.u1, uvDeskMat.v1), Vec2(uvDeskMat.u0, uvDeskMat.v1),
                    Vec3(0, 1.0f, 0));

        // --- Custom Mechanical Keyboard (Turned around to face the worker at -Z) ---
        float kbW = 0.18f;
        float kbFrontZ = -1.94f; // Farther from typist
        float kbRearZ  = -2.14f; // Closer to typist's hands
        float kbFrontY = -0.842f; // Tilted slightly up at the back
        float kbRearY  = -0.855f; // Lower near typist's wrists
        obj.addBox(cx - kbW, -0.858f, kbRearZ, cx + kbW, kbFrontY, kbFrontZ, uvSteel);
        obj.addQuad(Vec3(cx - kbW, kbRearY, kbRearZ), Vec3(cx + kbW, kbRearY, kbRearZ),
                    Vec3(cx + kbW, kbFrontY, kbFrontZ), Vec3(cx - kbW, kbFrontY, kbFrontZ),
                    Vec2(uvKeyboard.u0, uvKeyboard.v0), Vec2(uvKeyboard.u1, uvKeyboard.v0),
                    Vec2(uvKeyboard.u1, uvKeyboard.v1), Vec2(uvKeyboard.u0, uvKeyboard.v1),
                    Vec3(0, 1.0f, 0));

        // Ergonomic Mouse (Placed to the right of keyboard for typist: cx + kbW)
        obj.addBox(cx + kbW + 0.02f, -0.857f, -2.09f, cx + kbW + 0.075f, -0.835f, -1.99f, uvSteel);
    };

    // 1. Left AI Worker Setup (X = -1.06f) - VS Code IDE
    addAIWorkstation(-1.06f, uvScreenMain, false);

    // 2. Center AI Worker Setup (X = 0.00f) - System Telemetry & Loss Curves
    addAIWorkstation(0.00f, uvScreenSec, true);

    // 3. Right AI Worker Setup (X = +1.06f) - VS Code IDE
    addAIWorkstation(+1.06f, uvScreenMain, false);

    // --- High-End Custom PC Towers on Console Wings ---
    // Left Wing PC Tower (X = -1.70f to -1.45f)
    float pcW = 0.25f, pcH = 0.38f, pcD = 0.38f;
    float lpcX0 = -1.72f, lpcX1 = lpcX0 + pcW;
    float lpcY0 = -0.859f, lpcY1 = lpcY0 + pcH;
    float lpcZ0 = -2.15f, lpcZ1 = lpcZ0 + pcD;

    obj.addBox(lpcX0, lpcY0, lpcZ0, lpcX1, lpcY1, lpcZ1, uvSteel);
    // Front Face (+Z): 3 Glowing RGB Fans
    obj.addQuad(Vec3(lpcX0, lpcY0, lpcZ1 + 0.002f), Vec3(lpcX1, lpcY0, lpcZ1 + 0.002f),
                Vec3(lpcX1, lpcY1, lpcZ1 + 0.002f), Vec3(lpcX0, lpcY1, lpcZ1 + 0.002f),
                Vec2(uvPCFront.u0, uvPCFront.v0), Vec2(uvPCFront.u1, uvPCFront.v0),
                Vec2(uvPCFront.u1, uvPCFront.v1), Vec2(uvPCFront.u0, uvPCFront.v1),
                Vec3(0, 0, 1.0f));
    // Right Face (+X, facing inward): Tempered Glass Window with RTX 4090 GPU
    obj.addQuad(Vec3(lpcX1 + 0.002f, lpcY0, lpcZ1), Vec3(lpcX1 + 0.002f, lpcY0, lpcZ0),
                Vec3(lpcX1 + 0.002f, lpcY1, lpcZ0), Vec3(lpcX1 + 0.002f, lpcY1, lpcZ1),
                Vec2(uvPCSide.u0, uvPCSide.v0), Vec2(uvPCSide.u1, uvPCSide.v0),
                Vec2(uvPCSide.u1, uvPCSide.v1), Vec2(uvPCSide.u0, uvPCSide.v1),
                Vec3(1.0f, 0, 0));

    // Right Wing PC Tower (X = +1.47f to +1.72f)
    float rpcX0 = 1.47f, rpcX1 = rpcX0 + pcW;
    obj.addBox(rpcX0, lpcY0, lpcZ0, rpcX1, lpcY1, lpcZ1, uvSteel);
    // Front Face (+Z): 3 Glowing RGB Fans
    obj.addQuad(Vec3(rpcX0, lpcY0, lpcZ1 + 0.002f), Vec3(rpcX1, lpcY0, lpcZ1 + 0.002f),
                Vec3(rpcX1, lpcY1, lpcZ1 + 0.002f), Vec3(rpcX0, lpcY1, lpcZ1 + 0.002f),
                Vec2(uvPCFront.u0, uvPCFront.v0), Vec2(uvPCFront.u1, uvPCFront.v0),
                Vec2(uvPCFront.u1, uvPCFront.v1), Vec2(uvPCFront.u0, uvPCFront.v1),
                Vec3(0, 0, 1.0f));
    // Left Face (-X, facing inward): Tempered Glass Window with RTX 4090 GPU
    obj.addQuad(Vec3(rpcX0 - 0.002f, lpcY0, lpcZ0), Vec3(rpcX0 - 0.002f, lpcY0, lpcZ1),
                Vec3(rpcX0 - 0.002f, lpcY1, lpcZ1), Vec3(rpcX0 - 0.002f, lpcY1, lpcZ0),
                Vec2(uvPCSide.u0, uvPCSide.v0), Vec2(uvPCSide.u1, uvPCSide.v0),
                Vec2(uvPCSide.u1, uvPCSide.v1), Vec2(uvPCSide.u0, uvPCSide.v1),
                Vec3(-1.0f, 0, 0));

    // Studio Speakers on Console
    obj.addBox(-1.44f, -0.859f, -1.82f, -1.36f, -0.66f, -1.72f, uvSteel);
    obj.addQuad(Vec3(-1.44f, -0.859f, -1.718f), Vec3(-1.36f, -0.859f, -1.718f),
                Vec3(-1.36f, -0.66f, -1.718f), Vec3(-1.44f, -0.66f, -1.718f),
                Vec2(uvSpeaker.u0, uvSpeaker.v0), Vec2(uvSpeaker.u1, uvSpeaker.v0),
                Vec2(uvSpeaker.u1, uvSpeaker.v1), Vec2(uvSpeaker.u0, uvSpeaker.v1),
                Vec3(0, 0, 1.0f));

    obj.addBox(1.36f, -0.859f, -1.82f, 1.44f, -0.66f, -1.72f, uvSteel);
    obj.addQuad(Vec3(1.36f, -0.859f, -1.718f), Vec3(1.44f, -0.859f, -1.718f),
                Vec3(1.44f, -0.66f, -1.718f), Vec3(1.36f, -0.66f, -1.718f),
                Vec2(uvSpeaker.u0, uvSpeaker.v0), Vec2(uvSpeaker.u1, uvSpeaker.v0),
                Vec2(uvSpeaker.u1, uvSpeaker.v1), Vec2(uvSpeaker.u0, uvSpeaker.v1),
                Vec3(0, 0, 1.0f));

    // ------------------------------------------------------------------------
    // 9. Multi-Tier Bookshelf (Left Back Corner)
    // ------------------------------------------------------------------------
    float bsMinX = -2.35f, bsMaxX = -1.1f;
    float bsMinZ = -4.95f, bsMaxZ = -4.45f;
    obj.addBox(bsMinX, -1.50f, bsMinZ, bsMinX + 0.05f, 1.35f, bsMaxZ, uvDeskWood);
    obj.addBox(bsMaxX - 0.05f, -1.50f, bsMinZ, bsMaxX, 1.35f, bsMaxZ, uvDeskWood);
    obj.addBox(bsMinX, 1.30f, bsMinZ, bsMaxX, 1.35f, bsMaxZ, uvDeskWood);
    obj.addBox(bsMinX, -1.50f, bsMinZ, bsMaxX, 1.35f, bsMinZ + 0.03f, uvDeskWood);
    float shelfHeights[] = { -1.45f, -0.85f, -0.25f, 0.35f, 0.95f };
    for (float sy : shelfHeights) {
        obj.addBox(bsMinX + 0.05f, sy, bsMinZ + 0.03f, bsMaxX - 0.05f, sy + 0.04f, bsMaxZ, uvDeskWood);
    }
    obj.addBox(bsMinX + 0.08f, -0.81f, bsMinZ + 0.06f, bsMaxX - 0.15f, -0.32f, bsMaxZ - 0.05f, uvBooks);
    obj.addBox(bsMinX + 0.15f, -0.21f, bsMinZ + 0.06f, bsMaxX - 0.25f,  0.28f, bsMaxZ - 0.05f, uvBooks);
    // Cylinder trophy removed as requested in Image 2!

    // ------------------------------------------------------------------------
    // 10. Potted Office Plant (Beside the Window, Free of the Desk!)
    // As requested: "ต้นไม้โดนทับ ให้ปรับตัวแหน่งต้นไม้"
    // Repositioned to the open floor by the window at Z = -0.40f (1m clear of desk at -1.48f)
    // ------------------------------------------------------------------------
    float plantX = -2.05f, plantZ = -0.40f;
    obj.addCylinder(plantX, plantZ, -1.50f, -0.95f, 0.22f, 16, uvDrywall); // Ceramic pot
    obj.addCylinder(plantX, plantZ, -0.96f, -0.93f, 0.20f, 16, uvDeskWood); // Dark soil
    obj.addBox(plantX - 0.28f, -0.93f, plantZ - 0.28f, plantX + 0.28f, -0.25f, plantZ + 0.28f, uvFoliage);
    obj.addBox(plantX - 0.18f, -0.25f, plantZ - 0.18f, plantX + 0.18f,  0.18f, plantZ + 0.18f, uvFoliage);
    obj.addBox(plantX - 0.10f,  0.18f, plantZ - 0.10f, plantX + 0.10f,  0.42f, plantZ + 0.10f, uvFoliage);

    // ------------------------------------------------------------------------
    // 11. Wastebasket next to manager desk
    // ------------------------------------------------------------------------
    obj.addCylinder(2.52f, -3.55f, -1.50f, -1.15f, 0.12f, 12, uvSteel);

    obj.saveOBJ("Models/scene_room.obj");
    std::cout << "[generate_office] Saved Models/scene_room.obj (" << obj.vertices.size() << " vertices, " << obj.faces.size() << " faces)\n";
    std::cout << "[generate_office] Done!\n";
    return 0;
}
