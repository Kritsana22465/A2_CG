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
            // Front face (+Z)
            addQuad(Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ),
                    uv00, uv10, uv11, uv01, Vec3(0, 0, 1.0f * s));
            // Back face (-Z)
            addQuad(Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ),
                    uv00, uv10, uv11, uv01, Vec3(0, 0, -1.0f * s));
            // Left face (-X)
            addQuad(Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Vec3(minX, maxY, minZ),
                    uv00, uv10, uv11, uv01, Vec3(-1.0f * s, 0, 0));
            // Right face (+X)
            addQuad(Vec3(maxX, minY, maxZ), Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ),
                    uv00, uv10, uv11, uv01, Vec3(1.0f * s, 0, 0));
            // Top face (+Y)
            addQuad(Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ),
                    uv00, uv10, uv11, uv01, Vec3(0, 1.0f * s, 0));
            // Bottom face (-Y)
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

        out << "# Procedural Tech Office Scene Room with High-End Battlestation Computer\n";
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

int main() {
    std::cout << "[generate_office] Generating HD 2048x2048 textures and upgraded Computer Battlestation...\n";

    Image img;

    // ========================================================================
    // UV Layout Map (2048x2048)
    // [0..1023, 0..1023]       : Wood Parquet Flooring (0.0..0.5, 0.0..0.5)
    // [1024..2047, 0..1023]    : Agile Scrum Whiteboard (0.5..1.0, 0.0..0.5)
    // [0..1023, 1024..2047]    : Panoramic Window City Skyline View (0.0..0.5, 0.5..1.0)
    // [1024..1279, 1024..1535] : Primary Monitor (VS Code IDE HD) (0.50..0.625, 0.50..0.75)
    // [1280..1535, 1024..1535] : Secondary Monitor (System Telemetry & Graphs) (0.625..0.75, 0.50..0.75)
    // [1536..1791, 1024..1279] : PC Case Front Mesh with 3 Glowing RGB Fans (0.75..0.875, 0.50..0.625)
    // [1792..2047, 1024..1279] : PC Case Tempered Glass Side Window (GPU/RAM/Liquid Cooler) (0.875..1.0, 0.50..0.625)
    // [1536..1791, 1280..1535] : Custom Mechanical Keyboard & Stream Deck (0.75..0.875, 0.625..0.75)
    // [1792..2047, 1280..1535] : Studio Monitor Speakers & Headphone Earcups (0.875..1.0, 0.625..0.75)
    // [1024..1279, 1536..2047] : Office Door, Warning Plaque (0.50..0.625, 0.75..1.0)
    // [1280..1407, 1536..1663] : Office Wall Clock (0.625..0.6875, 0.75..0.8125)
    // [1280..1535, 1664..2047] : Tech Vision Poster (0.625..0.75, 0.8125..1.0)
    // [1536..1791, 1536..1791] : Bookshelf Spines & Binders (0.75..0.875, 0.75..0.875)
    // [1792..2047, 1536..1791] : Dark Executive Desk Wood (0.875..1.0, 0.75..0.875)
    // [1536..1791, 1792..1919] : Designer Area Rug (0.75..0.875, 0.875..0.9375)
    // [1792..2047, 1792..1855] : Caution Hazard Strip (0.875..1.0, 0.875..0.90625)
    // [1792..1919, 1856..1983] : Plant Foliage (0.875..0.9375, 0.90625..0.96875)
    // [1920..2047, 1856..1983] : Drywall Tint (0.9375..1.0, 0.90625..0.96875)
    // [1536..1791, 1920..2047] : Acoustic Ceiling Grid (0.75..0.875, 0.9375..1.0)
    // [1792..2047, 1984..2047] : Brushed Metallic Steel (0.875..1.0, 0.96875..1.0)
    // ========================================================================

    // ------------------------------------------------------------------------
    // 1. Wood Parquet Flooring [0..1023, 0..1023]
    // ------------------------------------------------------------------------
    img.fillRect(0, 0, 1023, 1023, 195, 155, 120);
    for (int y = 0; y < 1024; y += 64) {
        for (int x = 0; x < 1024; x += 128) {
            int offset = (y / 64) % 2 == 0 ? 0 : 64;
            int px = (x + offset) % 1024;
            uint8_t tint = 180 + ((x * 19 + y * 37) % 35);
            img.fillRect(px, y, (px + 127) % 1024, y + 60, tint, (uint8_t)(tint * 0.82f), (uint8_t)(tint * 0.62f));
            img.drawRectOutline(px, y, (px + 127) % 1024, y + 60, 2, 135, 100, 70);
        }
    }

    // ------------------------------------------------------------------------
    // 2. Agile Scrum Whiteboard [1024..2047, 0..1023]
    // ------------------------------------------------------------------------
    img.fillRect(1024, 0, 2047, 1023, 248, 250, 252);
    img.drawRectOutline(1024, 0, 2047, 1023, 16, 160, 165, 172);
    img.drawHLine(1040, 2030, 120, 40, 60, 120);
    img.drawVLine(1360, 120, 980, 180, 185, 190);
    img.drawVLine(1700, 120, 980, 180, 185, 190);

    img.drawString(1060, 40, "AI LAB SPRINT 42: PROD DEPLOYMENT", 4, 25, 45, 100);
    img.drawString(1120, 150, "TODO", 4, 180, 30, 30);
    img.drawString(1430, 150, "IN PROGRESS", 4, 200, 130, 20);
    img.drawString(1820, 150, "DONE", 4, 30, 150, 40);

    // Sticky Notes
    img.fillRect(1060, 220, 1200, 320, 255, 240, 130);
    img.drawString(1070, 240, "TRAIN LLM", 2, 50, 50, 50);
    img.drawString(1070, 270, "EPOCH 1000", 2, 50, 50, 50);

    img.fillRect(1210, 220, 1350, 320, 255, 170, 180);
    img.drawString(1220, 240, "FIX LOSS", 2, 50, 50, 50);
    img.drawString(1220, 270, "SPIKE #99", 2, 50, 50, 50);

    img.fillRect(1400, 220, 1540, 320, 255, 200, 100);
    img.drawString(1410, 240, "SUPERVISE", 2, 50, 50, 50);
    img.drawString(1410, 270, "AI WORKERS", 2, 50, 50, 50);

    img.fillRect(1550, 220, 1690, 320, 255, 255, 120);
    img.drawString(1560, 240, "PHONG BLINN", 2, 50, 50, 50);
    img.drawString(1560, 270, "SHADOW MAP", 2, 50, 50, 50);

    img.fillRect(1740, 220, 1880, 320, 180, 245, 180);
    img.drawString(1750, 240, "BATTLESTATION", 2, 50, 50, 50);
    img.drawString(1750, 270, "PC SETUP OK", 2, 50, 50, 50);

    img.fillRect(1890, 220, 2030, 320, 180, 245, 180);
    img.drawString(1900, 240, "BREW COFFEE", 2, 50, 50, 50);
    img.drawString(1900, 270, "100 PCT PASS", 2, 50, 50, 50);

    img.drawString(1080, 740, "DEADLINE: 11:59 PM TONIGHT!", 4, 220, 30, 30);
    img.drawString(1080, 800, "CRUNCH TIME - NO SLEEP!", 4, 220, 30, 30);

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
    // Clouds
    img.fillCircle(240, 1700, 80, 255, 255, 255);
    img.fillCircle(320, 1720, 100, 255, 255, 255);
    img.fillCircle(400, 1700, 70, 255, 255, 255);
    img.fillCircle(760, 1760, 90, 255, 255, 255);
    img.fillCircle(860, 1780, 110, 255, 255, 255);

    // City Skyscrapers
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
    // 4. Primary Monitor: VS Code IDE (HD) [1024..1279, 1024..1535]
    // ------------------------------------------------------------------------
    int sc1X0 = 1024, sc1Y0 = 1024, sc1X1 = 1279, sc1Y1 = 1535;
    img.fillRect(sc1X0, sc1Y0, sc1X1, sc1Y1, 30, 30, 30); // Background
    // Top Tab bar
    img.fillRect(sc1X0, sc1Y0, sc1X1, sc1Y0 + 28, 45, 45, 48);
    img.fillRect(sc1X0 + 10, sc1Y0 + 4, sc1X0 + 110, sc1Y0 + 26, 30, 30, 30);
    img.drawString(sc1X0 + 18, sc1Y0 + 10, "main.cpp", 1, 230, 230, 230);
    img.drawString(sc1X0 + 98, sc1Y0 + 10, "x", 1, 150, 150, 150);
    img.drawString(sc1X0 + 125, sc1Y0 + 10, "shader.frag", 1, 160, 160, 160);

    // Left Activity Bar & File Tree
    img.fillRect(sc1X0, sc1Y0 + 28, sc1X0 + 16, sc1Y1 - 20, 51, 51, 51);
    img.fillRect(sc1X0 + 16, sc1Y0 + 28, sc1X0 + 45, sc1Y1 - 20, 37, 37, 38);
    img.drawString(sc1X0 + 20, sc1Y0 + 34, "SRC", 1, 180, 180, 180);
    img.drawString(sc1X0 + 20, sc1Y0 + 46, "LIBS", 1, 150, 150, 150);
    img.drawString(sc1X0 + 20, sc1Y0 + 58, "SHAD", 1, 150, 150, 150);

    // Line Numbers & Code
    int codeX = sc1X0 + 52;
    int lineY = sc1Y0 + 36;
    int lineSpacing = 16;

    img.drawString(codeX, lineY, "01  #include <GL/glew.h>", 1, 160, 160, 160); lineY += lineSpacing;
    img.drawString(codeX, lineY, "02  #include \"Libs/Shader.h\"", 1, 206, 145, 120); lineY += lineSpacing;
    img.drawString(codeX, lineY, "03  // --- AI Worker Controller ---", 1, 106, 153, 85); lineY += lineSpacing;
    img.drawString(codeX, lineY, "04  void TrainModel() {", 1, 86, 156, 214); lineY += lineSpacing;
    img.drawString(codeX, lineY, "05    auto ai = LoadWorker();", 1, 220, 220, 170); lineY += lineSpacing;
    img.drawString(codeX, lineY, "06    ai->KneelAndCode();", 1, 78, 201, 176); lineY += lineSpacing;
    img.drawString(codeX, lineY, "07    float loss = 0.0001f;", 1, 181, 206, 168); lineY += lineSpacing;
    img.drawString(codeX, lineY, "08    if (loss > 0.05f) {", 1, 197, 134, 192); lineY += lineSpacing;
    img.drawString(codeX, lineY, "09      Boss->RaiseWhip();", 1, 220, 180, 100); lineY += lineSpacing;
    img.drawString(codeX, lineY, "10    }", 1, 220, 220, 220); lineY += lineSpacing;
    img.drawString(codeX, lineY, "11    RenderScene(view, proj);", 1, 220, 220, 170); lineY += lineSpacing;
    img.drawString(codeX, lineY, "12  }", 1, 86, 156, 214); lineY += lineSpacing;

    // Mini-map on right
    img.fillRect(sc1X1 - 18, sc1Y0 + 28, sc1X1 - 2, sc1Y1 - 20, 35, 35, 38);
    for (int my = sc1Y0 + 35; my < sc1Y1 - 40; my += 6) {
        img.drawHLine(sc1X1 - 16, sc1X1 - 6, my, 80, 120, 180);
    }

    // Terminal Panel at bottom
    img.fillRect(sc1X0, sc1Y1 - 120, sc1X1, sc1Y1 - 20, 24, 24, 24);
    img.drawHLine(sc1X0, sc1X1, sc1Y1 - 120, 60, 60, 65);
    img.drawString(sc1X0 + 8, sc1Y1 - 112, "TERMINAL: MSBUILD DEBUG x64", 1, 78, 201, 176);
    img.drawString(sc1X0 + 8, sc1Y1 - 96,  "[OK] Assignment2.exe built (0 warnings)", 1, 100, 255, 100);
    img.drawString(sc1X0 + 8, sc1Y1 - 80,  "[GL] Shaders compiled: Phong + Shadow PCF", 1, 220, 220, 220);
    img.drawString(sc1X0 + 8, sc1Y1 - 64,  "[GL] Framebuffer 1024x1024 active", 1, 220, 220, 220);
    img.drawString(sc1X0 + 8, sc1Y1 - 48,  "[FPS] 60.0 FPS | VSync: ON", 1, 100, 200, 255);
    img.drawString(sc1X0 + 8, sc1Y1 - 32,  "agent@dev-rig:~$ _", 1, 255, 255, 255);

    // Bottom Status Bar (Purple/Blue VS Code status)
    img.fillRect(sc1X0, sc1Y1 - 18, sc1X1, sc1Y1, 104, 33, 122);
    img.drawString(sc1X0 + 6, sc1Y1 - 14, "main*  Ln 12, Col 4  UTF-8  C++17  RTX4090", 1, 255, 255, 255);

    // ------------------------------------------------------------------------
    // 5. Secondary Monitor: Telemetry & Multi-Window Dashboard [1280..1535, 1024..1535]
    // ------------------------------------------------------------------------
    int sc2X0 = 1280, sc2Y0 = 1024, sc2X1 = 1535, sc2Y1 = 1535;
    img.fillRect(sc2X0, sc2Y0, sc2X1, sc2Y1, 18, 22, 32);
    // Header
    img.fillRect(sc2X0, sc2Y0, sc2X1, sc2Y0 + 26, 30, 38, 55);
    img.drawString(sc2X0 + 10, sc2Y0 + 8, "SYSTEM TELEMETRY - NODE AI-01", 1, 100, 210, 255);

    // Top Area: Circular Gauges (GPU & VRAM)
    int g1X = sc2X0 + 64, g1Y = sc2Y0 + 80;
    img.fillCircle(g1X, g1Y, 36, 30, 36, 50);
    img.drawRing(g1X, g1Y, 28, 36, 255, 80, 80); // Red high load ring
    img.drawString(g1X - 18, g1Y - 8, "GPU 99%", 1, 255, 255, 255);
    img.drawString(g1X - 16, g1Y + 6, "72 C", 1, 255, 180, 100);

    int g2X = sc2X0 + 180, g2Y = sc2Y0 + 80;
    img.fillCircle(g2X, g2Y, 36, 30, 36, 50);
    img.drawRing(g2X, g2Y, 28, 36, 50, 220, 130); // Green VRAM ring
    img.drawString(g2X - 22, g2Y - 8, "VRAM 92%", 1, 255, 255, 255);
    img.drawString(g2X - 22, g2Y + 6, "22/24 GB", 1, 180, 230, 255);

    // Middle Area: Training Loss Curve Plot
    img.drawRectOutline(sc2X0 + 12, sc2Y0 + 130, sc2X1 - 12, sc2Y0 + 280, 1, 50, 70, 100);
    img.drawString(sc2X0 + 20, sc2Y0 + 138, "EPOCH LOSS CURVE (STEP 10,000)", 1, 140, 200, 255);
    // Draw Grid lines
    for (int gy = sc2Y0 + 160; gy <= sc2Y0 + 260; gy += 25) {
        img.drawHLine(sc2X0 + 15, sc2X1 - 15, gy, 35, 48, 68);
    }
    // Plot exponential decay curve
    for (int px = sc2X0 + 20; px < sc2X1 - 20; ++px) {
        float normX = (float)(px - (sc2X0 + 20)) / (float)(sc2X1 - sc2X0 - 40);
        float normY = std::exp(-normX * 4.0f) * 0.85f + 0.08f;
        int py = (sc2Y0 + 265) - (int)(normY * 105.0f);
        img.setPixel(px, py, 50, 255, 140);
        img.setPixel(px, py + 1, 50, 255, 140);
        img.setPixel(px, py - 1, 0, 200, 100);
    }

    // Lower Middle Area: Network & CPU Specs
    img.drawString(sc2X0 + 16, sc2Y0 + 295, "CPU: AMD RYZEN 9 7950X (32 THREADS)", 1, 200, 210, 230);
    img.drawString(sc2X0 + 16, sc2Y0 + 312, "NET: 10 Gbps SFP+ | LATENCY: 0.8 ms", 1, 200, 210, 230);

    // Bottom Area: AI Live Agent Feed
    img.fillRect(sc2X0 + 12, sc2Y0 + 335, sc2X1 - 12, sc2Y1 - 15, 12, 16, 24);
    img.drawRectOutline(sc2X0 + 12, sc2Y0 + 335, sc2X1 - 12, sc2Y1 - 15, 1, 40, 60, 90);
    img.drawString(sc2X0 + 18, sc2Y0 + 345, "LIVE AI AGENT CLUSTER:", 1, 255, 215, 0);
    img.drawString(sc2X0 + 18, sc2Y0 + 365, "[AI-01] Optimizing 3D vertices...", 1, 80, 220, 120);
    img.drawString(sc2X0 + 18, sc2Y0 + 385, "[AI-02] Compiling Phong shaders...", 1, 80, 220, 120);
    img.drawString(sc2X0 + 18, sc2Y0 + 405, "[AI-03] Shadow map PCF pass: 100%", 1, 80, 220, 120);
    img.drawString(sc2X0 + 18, sc2Y0 + 425, "[BOSS] Supervision active. Keep coding!", 1, 255, 100, 100);
    img.drawString(sc2X0 + 18, sc2Y0 + 445, "[STATUS] All systems running optimal.", 1, 100, 200, 255);
    img.drawString(sc2X0 + 18, sc2Y0 + 465, "[CLUSTER] GPU Temp 72 C | Fans 100%", 1, 180, 180, 180);

    // ------------------------------------------------------------------------
    // 6. PC Case Front Mesh with 3 Glowing RGB Fans [1536..1791, 1024..1279]
    // ------------------------------------------------------------------------
    int pcfX0 = 1536, pcfY0 = 1024, pcfX1 = 1791, pcfY1 = 1279;
    img.fillRect(pcfX0, pcfY0, pcfX1, pcfY1, 22, 22, 24); // Dark mesh chassis
    // Hexagonal / mesh dot pattern
    for (int y = pcfY0; y <= pcfY1; y += 8) {
        for (int x = pcfX0; x <= pcfX1; x += 8) {
            img.setPixel(x + ((y / 8) % 2 ? 4 : 0), y, 38, 38, 42);
        }
    }
    // 3 Large Glowing RGB Halo-Ring Intake Fans
    int fanCentersY[] = { pcfY0 + 45, pcfY0 + 128, pcfY0 + 210 };
    int fanCenterX = (pcfX0 + pcfX1) / 2;
    for (int f = 0; f < 3; ++f) {
        int cy = fanCentersY[f];
        // Outer RGB Halo
        img.drawRing(fanCenterX, cy, 32, 38, 0, 230, 255); // Cyan glow
        img.drawRing(fanCenterX, cy, 28, 32, 255, 0, 180); // Magenta accent
        // Fan hub & blades
        img.fillCircle(fanCenterX, cy, 12, 45, 45, 50);
        img.drawRing(fanCenterX, cy, 12, 14, 255, 255, 255); // Center LED ring
    }
    // Top Power Button & USB ports
    img.fillCircle(fanCenterX - 40, pcfY0 + 12, 5, 200, 220, 255); // Power button
    img.fillRect(fanCenterX - 20, pcfY0 + 9, fanCenterX - 10, pcfY0 + 15, 50, 150, 255); // USB 3.0
    img.fillRect(fanCenterX + 10, pcfY0 + 9, fanCenterX + 20, pcfY0 + 15, 50, 150, 255);

    // ------------------------------------------------------------------------
    // 7. PC Case Tempered Glass Side Window [1792..2047, 1024..1279]
    // ------------------------------------------------------------------------
    int pcsX0 = 1792, pcsY0 = 1024, pcsX1 = 2047, pcsY1 = 1279;
    img.fillRect(pcsX0, pcsY0, pcsX1, pcsY1, 15, 17, 22); // Dark interior
    img.drawRectOutline(pcsX0, pcsY0, pcsX1, pcsY1, 6, 40, 42, 48); // Case frame

    // Motherboard PCB & Heatsinks
    img.fillRect(pcsX0 + 20, pcsY0 + 20, pcsX1 - 20, pcsY1 - 20, 28, 30, 36);
    // Silver Motherboard VRM Heatsinks
    img.fillRect(pcsX0 + 35, pcsY0 + 35, pcsX0 + 75, pcsY0 + 95, 140, 145, 155);
    img.fillRect(pcsX0 + 80, pcsY0 + 35, pcsX0 + 140, pcsY0 + 60, 140, 145, 155);

    // CPU Liquid Cooler AIO Block (Glowing circular infinity mirror)
    int cpuX = pcsX0 + 105, cpuY = pcsY0 + 95;
    img.fillCircle(cpuX, cpuY, 26, 10, 10, 15);
    img.drawRing(cpuX, cpuY, 20, 26, 0, 240, 255); // Cyan ring
    img.drawRing(cpuX, cpuY, 14, 18, 255, 0, 150); // Pink inner ring
    img.drawString(cpuX - 14, cpuY - 4, "65C", 1, 255, 255, 255);
    // Braided Liquid Coolant Tubes
    for (int t = 0; t < 45; ++t) {
        img.setPixel(cpuX + 22 + t, cpuY - 15 - t / 2, 45, 45, 50);
        img.setPixel(cpuX + 22 + t, cpuY - 14 - t / 2, 60, 60, 65);
        img.setPixel(cpuX + 22 + t, cpuY - 5 - t / 2, 45, 45, 50);
        img.setPixel(cpuX + 22 + t, cpuY - 4 - t / 2, 60, 60, 65);
    }

    // Dual RGB RAM Sticks
    int ramX = pcsX0 + 150, ramY = pcsY0 + 65;
    img.fillRect(ramX, ramY, ramX + 8, ramY + 55, 255, 100, 200); // RAM 1 RGB
    img.fillRect(ramX + 12, ramY, ramX + 20, ramY + 55, 100, 200, 255); // RAM 2 RGB

    // Massive GeForce RTX 4090 GPU (Chunky with triple fans and glowing logo)
    int gpuX0 = pcsX0 + 25, gpuY0 = pcsY0 + 145, gpuX1 = pcsX1 - 30, gpuY1 = pcsY0 + 215;
    img.fillRect(gpuX0, gpuY0, gpuX1, gpuY1, 35, 38, 44); // GPU Shroud
    img.drawRectOutline(gpuX0, gpuY0, gpuX1, gpuY1, 2, 80, 85, 95);
    // Glowing GPU Logo
    img.fillRect(gpuX0 + 15, gpuY0 + 8, gpuX0 + 120, gpuY0 + 22, 10, 10, 12);
    img.drawString(gpuX0 + 20, gpuY0 + 12, "GEFORCE RTX 4090", 1, 118, 185, 0); // NVIDIA Green!
    // 3 GPU Cooling Fans
    int gpuFanCenters[] = { gpuX0 + 45, gpuX0 + 100, gpuX0 + 155 };
    for (int gx : gpuFanCenters) {
        img.fillCircle(gx, gpuY0 + 44, 20, 20, 22, 26);
        img.drawRing(gx, gpuY0 + 44, 16, 20, 120, 130, 140);
        img.fillCircle(gx, gpuY0 + 44, 6, 70, 75, 85);
    }

    // Power Supply Shroud at bottom
    img.fillRect(pcsX0 + 10, pcsY1 - 35, pcsX1 - 10, pcsY1 - 8, 25, 27, 32);
    img.drawString(pcsX0 + 30, pcsY1 - 25, "1200W PLATINUM ATX 3.0", 1, 120, 120, 130);

    // ------------------------------------------------------------------------
    // 8. Custom Mechanical Keyboard & Stream Deck [1536..1791, 1280..1535]
    // ------------------------------------------------------------------------
    int kbX0 = 1536, kbY0 = 1280, kbX1 = 1791, kbY1 = 1535;
    img.fillRect(kbX0, kbY0, kbX1, kbY1, 35, 37, 42); // Dark aluminum case
    // Draw individual keycaps (75% compact layout)
    int rowY = kbY0 + 15;
    for (int r = 0; r < 5; ++r) {
        int curKx = kbX0 + 15;
        for (int k = 0; k < 14; ++k) {
            uint8_t kr = 220, kg = 220, kb = 225; // White alphas
            if (r == 0 || k == 0 || k >= 12) {
                kr = 70; kg = 75; kb = 85; // Dark modifiers
            }
            if (r == 0 && k == 0) { kr = 255; kg = 90; kb = 30; } // Orange ESC key!
            if (r == 2 && k == 12) { kr = 0; kg = 180; kb = 230; } // Cyan Enter key!
            img.fillRect(curKx, rowY, curKx + 13, rowY + 12, kr, kg, kb);
            img.drawRectOutline(curKx, rowY, curKx + 13, rowY + 12, 1, 25, 25, 30);
            curKx += 16;
        }
        rowY += 15;
    }
    // Spacebar
    img.fillRect(kbX0 + 80, kbY0 + 92, kbX0 + 165, kbY0 + 105, 220, 220, 225);
    img.drawRectOutline(kbX0 + 80, kbY0 + 92, kbX0 + 165, kbY0 + 105, 1, 25, 25, 30);

    // Stream Deck (15-key Macro Pad) [kbX0 + 15..kbX0 + 130, kbY0 + 135..kbY1 - 15]
    img.fillRect(kbX0 + 15, kbY0 + 135, kbX0 + 130, kbY1 - 15, 20, 20, 25);
    img.drawRectOutline(kbX0 + 15, kbY0 + 135, kbX0 + 130, kbY1 - 15, 2, 80, 85, 95);
    for (int sy = 0; sy < 3; ++sy) {
        for (int sx = 0; sx < 5; ++sx) {
            int kx = kbX0 + 22 + sx * 21;
            int ky = kbY0 + 144 + sy * 30;
            uint8_t sr = (sx * 55) % 255;
            uint8_t sg = (sy * 85 + 80) % 255;
            uint8_t sb = 220;
            img.fillRect(kx, ky, kx + 16, ky + 22, sr, sg, sb);
            img.drawRectOutline(kx, ky, kx + 16, ky + 22, 1, 10, 10, 15);
        }
    }

    // Gaming Mouse Texture [kbX0 + 150..kbX1 - 15, kbY0 + 135..kbY1 - 15]
    img.fillRect(kbX0 + 155, kbY0 + 135, kbX1 - 20, kbY1 - 15, 28, 30, 35);
    // Mouse buttons divider
    img.drawVLine(kbX0 + 195, kbY0 + 140, kbY0 + 195, 15, 15, 20);
    // RGB Scroll Wheel
    img.fillRect(kbX0 + 191, kbY0 + 155, kbX0 + 199, kbY0 + 180, 0, 240, 255);
    img.drawString(kbX0 + 175, kbY1 - 40, "DPI: 3200", 1, 180, 180, 200);

    // ------------------------------------------------------------------------
    // 9. Studio Monitor Speakers & Headphone Earcups [1792..2047, 1280..1535]
    // ------------------------------------------------------------------------
    int spkX0 = 1792, spkY0 = 1280, spkX1 = 2047, spkY1 = 1535;
    img.fillRect(spkX0, spkY0, spkX1, spkY1, 32, 34, 38); // Matte black speaker cab
    img.drawRectOutline(spkX0, spkY0, spkX1, spkY1, 4, 18, 18, 22);
    // Tweeter (Top small cone)
    int spkMidX = (spkX0 + spkX1) / 2;
    img.fillCircle(spkMidX, spkY0 + 60, 24, 20, 22, 26);
    img.fillCircle(spkMidX, spkY0 + 60, 12, 180, 185, 195); // Silver dome
    // Woofer (Bottom large cone with yellow/silver Kevlar cone)
    img.fillCircle(spkMidX, spkY0 + 160, 60, 225, 190, 50); // Kevlar yellow
    img.drawRing(spkMidX, spkY0 + 160, 48, 60, 25, 25, 30); // Rubber surround
    img.fillCircle(spkMidX, spkY0 + 160, 20, 20, 20, 25); // Dust cap
    // Bass reflex port
    img.fillCircle(spkMidX, spkY0 + 232, 12, 10, 10, 12);

    // ------------------------------------------------------------------------
    // 10. Office Door, Clock, Poster, Materials [1024..2047, 1536..2047]
    // ------------------------------------------------------------------------
    // Door panel [1024..1279, 1536..2047]
    img.fillRect(1024, 1536, 1279, 2047, 140, 100, 70);
    img.drawRectOutline(1024, 1536, 1279, 2047, 8, 90, 60, 40);
    img.drawRectOutline(1048, 1560, 1255, 1680, 6, 100, 70, 45);
    img.drawRectOutline(1048, 1710, 1255, 1840, 6, 100, 70, 45);
    img.drawRectOutline(1048, 1870, 1255, 2010, 6, 100, 70, 45);
    // Door Plaque
    img.fillRect(1060, 1750, 1240, 1810, 30, 30, 35);
    img.drawRectOutline(1060, 1750, 1240, 1810, 2, 200, 180, 80);
    img.drawString(1080, 1765, "AI LAB 01", 2, 255, 255, 255);
    img.drawString(1080, 1788, "RESTRICTED", 2, 255, 80, 80);

    // Clock Face [1280..1407, 1536..1663] (128x128)
    int clkX = 1344, clkY = 1600;
    img.fillCircle(clkX, clkY, 60, 245, 245, 245);
    img.drawRing(clkX, clkY, 56, 62, 40, 40, 45);
    img.fillRect(clkX - 2, clkY - 54, clkX + 2, clkY - 40, 20, 20, 20); // 12
    img.fillRect(clkX - 2, clkY + 40, clkX + 2, clkY + 54, 20, 20, 20); // 6
    img.fillRect(clkX - 54, clkY - 2, clkX - 40, clkY + 2, 20, 20, 20); // 9
    img.fillRect(clkX + 40, clkY - 2, clkX + 54, clkY + 2, 20, 20, 20); // 3
    img.drawVLine(clkX, clkY - 35, clkY, 10, 10, 10); // Hour
    img.drawHLine(clkX - 25, clkX, clkY - 30, 220, 20, 20); // Minute (11:55)

    // Vision Poster [1280..1535, 1664..2047]
    img.fillRect(1280, 1664, 1535, 2047, 20, 25, 35);
    img.drawRectOutline(1280, 1664, 1535, 2047, 12, 210, 180, 90);
    img.drawString(1300, 1720, "INNOVATION", 3, 255, 215, 0);
    img.drawString(1310, 1780, "KEEP CALM", 3, 240, 240, 250);
    img.drawString(1340, 1830, "AND", 3, 240, 240, 250);
    img.drawString(1300, 1880, "SHIP THE AI", 3, 100, 220, 255);
    img.drawString(1310, 1980, "DEV CORP 2026", 2, 150, 160, 180);

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

    // Carpet Rug [1536..1791, 1792..1919]
    img.fillRect(1536, 1792, 1791, 1919, 40, 50, 68);
    for (int y = 1792; y < 1920; y += 16) {
        for (int x = 1536; x < 1792; x += 16) {
            if (((x / 16) + (y / 16)) % 2 == 0) {
                img.fillRect(x, y, x + 15, y + 15, 52, 65, 86);
            }
        }
    }

    // Caution Strip [1792..2047, 1792..1855]
    for (int x = 1792; x < 2048; ++x) {
        bool yellow = ((x + 20) / 24) % 2 == 0;
        uint8_t r = yellow ? 245 : 25;
        uint8_t g = yellow ? 210 : 25;
        uint8_t b = yellow ? 20 : 25;
        img.drawVLine(x, 1792, 1855, r, g, b);
    }
    img.drawString(1820, 1815, "CAUTION: AI WORKER ZONE", 2, 10, 10, 10);

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

    // UV Coordinates (Normalized 0.0 to 1.0)
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
    UVBox uvRug         = { 0.75f,    0.875f,   0.875f,   0.9375f };
    UVBox uvCaution     = { 0.875f,   0.875f,   1.0f,     0.90625f };
    UVBox uvFoliage     = { 0.875f,   0.90625f, 0.9375f,  0.96875f };
    UVBox uvDrywall     = { 0.9375f,  0.90625f, 1.0f,     0.96875f };
    UVBox uvCeiling     = { 0.75f,    0.9375f,  0.875f,   1.0f };
    UVBox uvSteel       = { 0.875f,   0.96875f, 1.0f,     1.0f };

    // 1. Room Shell (X: -2.5..4.5, Y: -1.5..1.7, Z: -5.0..2.0)
    // Floor
    obj.addQuad(Vec3(-2.5f, -1.5f,  2.0f), Vec3( 4.5f, -1.5f,  2.0f),
                Vec3( 4.5f, -1.5f, -5.0f), Vec3(-2.5f, -1.5f, -5.0f),
                Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(1.0f, 1.0f), Vec2(0.0f, 1.0f),
                Vec3(0, 1.0f, 0));
    // Ceiling
    obj.addQuad(Vec3(-2.5f, 1.7f, -5.0f), Vec3( 4.5f, 1.7f, -5.0f),
                Vec3( 4.5f, 1.7f,  2.0f), Vec3(-2.5f, 1.7f,  2.0f),
                Vec2(uvCeiling.u0, uvCeiling.v0), Vec2(uvCeiling.u1, uvCeiling.v0),
                Vec2(uvCeiling.u1, uvCeiling.v1), Vec2(uvCeiling.u0, uvCeiling.v1),
                Vec3(0, -1.0f, 0));
    // Back Wall (Z = -5.0)
    obj.addQuad(Vec3(-2.5f, -1.5f, -5.0f), Vec3( 4.5f, -1.5f, -5.0f),
                Vec3( 4.5f,  1.7f, -5.0f), Vec3(-2.5f,  1.7f, -5.0f),
                Vec2(uvDrywall.u0, uvDrywall.v0), Vec2(uvDrywall.u1, uvDrywall.v0),
                Vec2(uvDrywall.u1, uvDrywall.v1), Vec2(uvDrywall.u0, uvDrywall.v1),
                Vec3(0, 0, 1.0f));
    // Left Wall (X = -2.5)
    obj.addQuad(Vec3(-2.5f, -1.5f,  2.0f), Vec3(-2.5f, -1.5f, -5.0f),
                Vec3(-2.5f,  1.7f, -5.0f), Vec3(-2.5f,  1.7f,  2.0f),
                Vec2(uvDrywall.u0, uvDrywall.v0), Vec2(uvDrywall.u1, uvDrywall.v0),
                Vec2(uvDrywall.u1, uvDrywall.v1), Vec2(uvDrywall.u0, uvDrywall.v1),
                Vec3(1.0f, 0, 0));
    // Right Wall (X = 4.5)
    obj.addQuad(Vec3( 4.5f, -1.5f, -5.0f), Vec3( 4.5f, -1.5f,  2.0f),
                Vec3( 4.5f,  1.7f,  2.0f), Vec3( 4.5f,  1.7f, -5.0f),
                Vec2(uvDrywall.u0, uvDrywall.v0), Vec2(uvDrywall.u1, uvDrywall.v0),
                Vec2(uvDrywall.u1, uvDrywall.v1), Vec2(uvDrywall.u0, uvDrywall.v1),
                Vec3(-1.0f, 0, 0));

    // Baseboard moldings
    obj.addBox(-2.48f, -1.5f, -4.98f,  4.48f, -1.42f, -4.95f, uvDeskWood);
    obj.addBox(-2.48f, -1.5f, -4.98f, -2.45f, -1.42f,  1.98f, uvDeskWood);
    obj.addBox( 4.45f, -1.5f, -4.98f,  4.48f, -1.42f,  1.98f, uvDeskWood);

    // 2. Large Panoramic Window (Left Wall)
    obj.addBox(-2.49f, -0.62f, -4.2f, -2.44f, -0.58f, -0.6f, uvSteel);
    obj.addBox(-2.49f, -0.60f, -4.2f, -2.46f,  1.30f, -4.15f, uvSteel);
    obj.addBox(-2.49f, -0.60f, -0.65f, -2.46f, 1.30f, -0.60f, uvSteel);
    obj.addBox(-2.49f,  1.25f, -4.2f, -2.46f,  1.30f, -0.6f, uvSteel);
    obj.addBox(-2.49f,  0.35f, -4.2f, -2.47f,  0.40f, -0.6f, uvSteel);
    obj.addBox(-2.49f, -0.60f, -2.42f, -2.47f, 1.30f, -2.38f, uvSteel);
    obj.addQuad(Vec3(-2.48f, -0.58f, -0.65f), Vec3(-2.48f, -0.58f, -4.15f),
                Vec3(-2.48f,  1.25f, -4.15f), Vec3(-2.48f,  1.25f, -0.65f),
                Vec2(uvWindow.u0, uvWindow.v0), Vec2(uvWindow.u1, uvWindow.v0),
                Vec2(uvWindow.u1, uvWindow.v1), Vec2(uvWindow.u0, uvWindow.v1),
                Vec3(1.0f, 0, 0));

    // 3. Office Door (Right Wall)
    obj.addBox(4.46f, -1.5f, -0.65f, 4.49f, 1.15f, 0.65f, uvDeskWood);
    obj.addQuad(Vec3(4.47f, -1.5f, -0.6f), Vec3(4.47f, -1.5f,  0.6f),
                Vec3(4.47f,  1.1f,  0.6f), Vec3(4.47f,  1.1f, -0.6f),
                Vec2(uvDoor.u0, uvDoor.v0), Vec2(uvDoor.u1, uvDoor.v0),
                Vec2(uvDoor.u1, uvDoor.v1), Vec2(uvDoor.u0, uvDoor.v1),
                Vec3(-1.0f, 0, 0));
    obj.addBox(4.42f, -0.3f, 0.42f, 4.47f, -0.26f, 0.54f, uvSteel);

    // 4. Agile Scrum Whiteboard (Back Wall Center)
    obj.addBox(-0.52f, -0.22f, -4.98f, 2.22f, 1.17f, -4.95f, uvSteel);
    obj.addBox(-0.55f, -0.25f, -4.94f, 2.25f, -0.22f, -4.88f, uvSteel);
    obj.addQuad(Vec3(-0.5f, -0.2f, -4.95f), Vec3( 2.2f, -0.2f, -4.95f),
                Vec3( 2.2f,  1.15f, -4.95f), Vec3(-0.5f,  1.15f, -4.95f),
                Vec2(uvWhiteboard.u0, uvWhiteboard.v0), Vec2(uvWhiteboard.u1, uvWhiteboard.v0),
                Vec2(uvWhiteboard.u1, uvWhiteboard.v1), Vec2(uvWhiteboard.u0, uvWhiteboard.v1),
                Vec3(0, 0, 1.0f));

    // 5. Inspirational Vision Poster
    obj.addBox(2.68f, 0.28f, -4.98f, 3.82f, 1.22f, -4.96f, uvSteel);
    obj.addQuad(Vec3(2.7f, 0.3f, -4.96f), Vec3(3.8f, 0.3f, -4.96f),
                Vec3(3.8f, 1.2f, -4.96f), Vec3(2.7f, 1.2f, -4.96f),
                Vec2(uvPoster.u0, uvPoster.v0), Vec2(uvPoster.u1, uvPoster.v0),
                Vec2(uvPoster.u1, uvPoster.v1), Vec2(uvPoster.u0, uvPoster.v1),
                Vec3(0, 0, 1.0f));

    // 6. Wall Clock
    obj.addBox(0.68f, 1.28f, -4.98f, 1.12f, 1.52f, -4.96f, uvSteel);
    obj.addQuad(Vec3(0.7f, 1.3f, -4.96f), Vec3(1.1f, 1.3f, -4.96f),
                Vec3(1.1f, 1.5f, -4.96f), Vec3(0.7f, 1.5f, -4.96f),
                Vec2(uvClock.u0, uvClock.v0), Vec2(uvClock.u1, uvClock.v0),
                Vec2(uvClock.u1, uvClock.v1), Vec2(uvClock.u0, uvClock.v1),
                Vec3(0, 0, 1.0f));

    // 7. Executive Workstation Desk (Right Side)
    // Tabletop
    obj.addBox(2.6f, -0.78f, -4.3f, 4.25f, -0.72f, -3.4f, uvDeskWood);
    // Metal Legs
    obj.addBox(2.64f, -1.5f, -4.26f, 2.70f, -0.78f, -4.20f, uvSteel);
    obj.addBox(2.64f, -1.5f, -3.50f, 2.70f, -0.78f, -3.44f, uvSteel);
    // 3-Drawer Unit on Right
    obj.addBox(3.6f, -1.5f, -4.25f, 4.20f, -0.78f, -3.45f, uvDeskWood);
    obj.addBox(3.56f, -0.95f, -3.88f, 3.60f, -0.92f, -3.78f, uvSteel);
    obj.addBox(3.56f, -1.15f, -3.88f, 3.60f, -1.12f, -3.78f, uvSteel);
    obj.addBox(3.56f, -1.35f, -3.88f, 3.60f, -1.32f, -3.78f, uvSteel);

    // ========================================================================
    // 8. UPGRADED HIGH-END BATTLESTATION COMPUTER SETUP
    // ========================================================================

    // --- A. DUAL HEAVY-DUTY VESA MONITOR ARM SYSTEM ---
    // Desk C-Clamp mounted at the rear edge of desk
    obj.addBox(3.32f, -0.74f, -4.26f, 3.40f, -0.70f, -4.18f, uvSteel);
    // Vertical heavy steel riser pole
    obj.addCylinder(3.36f, -4.22f, -0.70f, -0.32f, 0.022f, 10, uvSteel);
    // Dual Articulated Mechanical Arms (Joints & Tubes)
    // Left arm to primary monitor
    obj.addBox(3.14f, -0.40f, -4.18f, 3.36f, -0.36f, -4.14f, uvSteel);
    obj.addBox(3.12f, -0.42f, -4.14f, 3.16f, -0.34f, -4.08f, uvSteel); // VESA plate
    // Right arm to secondary monitor
    obj.addBox(3.36f, -0.40f, -4.18f, 3.68f, -0.36f, -4.10f, uvSteel);
    obj.addBox(3.66f, -0.42f, -4.10f, 3.70f, -0.34f, -4.04f, uvSteel); // VESA plate

    // --- B. PRIMARY MONITOR (Center-Left: VS Code IDE) ---
    // Monitor 1 Outer Bezel (Ultra-slim borderless design)
    obj.addBox(2.80f, -0.58f, -4.07f, 3.45f, -0.16f, -4.03f, uvSteel);
    // Monitor 1 Screen Quad (High-Res VS Code)
    obj.addQuad(Vec3(2.81f, -0.57f, -4.025f), Vec3(3.44f, -0.57f, -4.025f),
                Vec3(3.44f, -0.17f, -4.025f), Vec3(2.81f, -0.17f, -4.025f),
                Vec2(uvScreenMain.u0, uvScreenMain.v0), Vec2(uvScreenMain.u1, uvScreenMain.v0),
                Vec2(uvScreenMain.u1, uvScreenMain.v1), Vec2(uvScreenMain.u0, uvScreenMain.v1),
                Vec3(0, 0, 1.0f));
    // Screen Light Bar (Monitor Lightbar clamped on top)
    obj.addCylinder(3.125f, -4.05f, -0.16f, -0.13f, 0.012f, 8, uvSteel); // Bar clamp
    obj.addCylinder(3.125f, -4.03f, -0.14f, -0.13f, 0.16f, 10, uvSteel); // Horizontal LED tube

    // --- C. SECONDARY MONITOR (Center-Right: Telemetry Dashboard, Angled 18 deg) ---
    // We model the angled secondary monitor cleanly using quads
    float m2L_X = 3.47f, m2L_Z = -4.05f;
    float m2R_X = 4.04f, m2R_Z = -3.87f;
    float m2B_Y = -0.58f, m2T_Y = -0.16f;
    float m2Thick = 0.035f;

    // Normal for 18-degree angled monitor
    float angleRad = 18.0f * 3.14159265f / 180.0f;
    Vec3 m2Norm(-std::sin(angleRad), 0, std::cos(angleRad));

    // Screen Quad
    obj.addQuad(Vec3(m2L_X, m2B_Y, m2L_Z), Vec3(m2R_X, m2B_Y, m2R_Z),
                Vec3(m2R_X, m2T_Y, m2R_Z), Vec3(m2L_X, m2T_Y, m2L_Z),
                Vec2(uvScreenSec.u0, uvScreenSec.v0), Vec2(uvScreenSec.u1, uvScreenSec.v0),
                Vec2(uvScreenSec.u1, uvScreenSec.v1), Vec2(uvScreenSec.u0, uvScreenSec.v1),
                m2Norm);
    // Back Cover Quad
    obj.addQuad(Vec3(m2R_X, m2B_Y, m2R_Z - m2Thick), Vec3(m2L_X, m2B_Y, m2L_Z - m2Thick),
                Vec3(m2L_X, m2T_Y, m2L_Z - m2Thick), Vec3(m2R_X, m2T_Y, m2R_Z - m2Thick),
                Vec2(uvSteel.u0, uvSteel.v0), Vec2(uvSteel.u1, uvSteel.v0),
                Vec2(uvSteel.u1, uvSteel.v1), Vec2(uvSteel.u0, uvSteel.v1),
                Vec3(-m2Norm.x, 0, -m2Norm.z));
    // Top & Bottom bezels
    obj.addQuad(Vec3(m2L_X, m2T_Y, m2L_Z), Vec3(m2R_X, m2T_Y, m2R_Z),
                Vec3(m2R_X, m2T_Y, m2R_Z - m2Thick), Vec3(m2L_X, m2T_Y, m2L_Z - m2Thick),
                Vec2(uvSteel.u0, uvSteel.v0), Vec2(uvSteel.u1, uvSteel.v0),
                Vec2(uvSteel.u1, uvSteel.v1), Vec2(uvSteel.u0, uvSteel.v1),
                Vec3(0, 1.0f, 0));
    obj.addQuad(Vec3(m2L_X, m2B_Y, m2L_Z - m2Thick), Vec3(m2R_X, m2B_Y, m2R_Z - m2Thick),
                Vec3(m2R_X, m2B_Y, m2R_Z), Vec3(m2L_X, m2B_Y, m2L_Z),
                Vec2(uvSteel.u0, uvSteel.v0), Vec2(uvSteel.u1, uvSteel.v0),
                Vec2(uvSteel.u1, uvSteel.v1), Vec2(uvSteel.u0, uvSteel.v1),
                Vec3(0, -1.0f, 0));

    // Webcam on Secondary Monitor
    obj.addBox(3.72f, -0.155f, -3.98f, 3.79f, -0.125f, -3.93f, uvSteel);

    // --- D. HIGH-END CUSTOM PC TOWER (Right Side of Desk) ---
    float pcMinX = 3.92f, pcMaxX = 4.22f; // Width 0.30m
    float pcMinY = -0.705f, pcMaxY = -0.24f; // Height 0.465m
    float pcMinZ = -4.15f, pcMaxZ = -3.65f; // Depth 0.50m

    // 4 Rubber Case Feet
    obj.addCylinder(pcMinX + 0.04f, pcMinZ + 0.05f, -0.72f, pcMinY, 0.015f, 6, uvSteel);
    obj.addCylinder(pcMaxX - 0.04f, pcMinZ + 0.05f, -0.72f, pcMinY, 0.015f, 6, uvSteel);
    obj.addCylinder(pcMinX + 0.04f, pcMaxZ - 0.05f, -0.72f, pcMinY, 0.015f, 6, uvSteel);
    obj.addCylinder(pcMaxX - 0.04f, pcMaxZ - 0.05f, -0.72f, pcMinY, 0.015f, 6, uvSteel);

    // Case Chassis (Top, Bottom, Right, Back)
    // Top
    obj.addQuad(Vec3(pcMinX, pcMaxY, pcMaxZ), Vec3(pcMaxX, pcMaxY, pcMaxZ),
                Vec3(pcMaxX, pcMaxY, pcMinZ), Vec3(pcMinX, pcMaxY, pcMinZ),
                Vec2(uvSteel.u0, uvSteel.v0), Vec2(uvSteel.u1, uvSteel.v0),
                Vec2(uvSteel.u1, uvSteel.v1), Vec2(uvSteel.u0, uvSteel.v1),
                Vec3(0, 1.0f, 0));
    // Bottom
    obj.addQuad(Vec3(pcMinX, pcMinY, pcMinZ), Vec3(pcMaxX, pcMinY, pcMinZ),
                Vec3(pcMaxX, pcMinY, pcMaxZ), Vec3(pcMinX, pcMinY, pcMaxZ),
                Vec2(uvSteel.u0, uvSteel.v0), Vec2(uvSteel.u1, uvSteel.v0),
                Vec2(uvSteel.u1, uvSteel.v1), Vec2(uvSteel.u0, uvSteel.v1),
                Vec3(0, -1.0f, 0));
    // Right panel (+X face)
    obj.addQuad(Vec3(pcMaxX, pcMinY, pcMaxZ), Vec3(pcMaxX, pcMinY, pcMinZ),
                Vec3(pcMaxX, pcMaxY, pcMinZ), Vec3(pcMaxX, pcMaxY, pcMaxZ),
                Vec2(uvSteel.u0, uvSteel.v0), Vec2(uvSteel.u1, uvSteel.v0),
                Vec2(uvSteel.u1, uvSteel.v1), Vec2(uvSteel.u0, uvSteel.v1),
                Vec3(1.0f, 0, 0));
    // Back panel (-Z face)
    obj.addQuad(Vec3(pcMaxX, pcMinY, pcMinZ), Vec3(pcMinX, pcMinY, pcMinZ),
                Vec3(pcMinX, pcMaxY, pcMinZ), Vec3(pcMaxX, pcMaxY, pcMinZ),
                Vec2(uvSteel.u0, uvSteel.v0), Vec2(uvSteel.u1, uvSteel.v0),
                Vec2(uvSteel.u1, uvSteel.v1), Vec2(uvSteel.u0, uvSteel.v1),
                Vec3(0, 0, -1.0f));

    // Front Panel (+Z face: 3 Glowing RGB Fans!)
    obj.addQuad(Vec3(pcMinX, pcMinY, pcMaxZ), Vec3(pcMaxX, pcMinY, pcMaxZ),
                Vec3(pcMaxX, pcMaxY, pcMaxZ), Vec3(pcMinX, pcMaxY, pcMaxZ),
                Vec2(uvPCFront.u0, uvPCFront.v0), Vec2(uvPCFront.u1, uvPCFront.v0),
                Vec2(uvPCFront.u1, uvPCFront.v1), Vec2(uvPCFront.u0, uvPCFront.v1),
                Vec3(0, 0, 1.0f));

    // Left Panel (-X face: Tempered Glass Window showing GPU / Cooler / RAM!)
    obj.addQuad(Vec3(pcMinX, pcMinY, pcMinZ), Vec3(pcMinX, pcMinY, pcMaxZ),
                Vec3(pcMinX, pcMaxY, pcMaxZ), Vec3(pcMinX, pcMaxY, pcMinZ),
                Vec2(uvPCSide.u0, uvPCSide.v0), Vec2(uvPCSide.u1, uvPCSide.v0),
                Vec2(uvPCSide.u1, uvPCSide.v1), Vec2(uvPCSide.u0, uvPCSide.v1),
                Vec3(-1.0f, 0, 0));

    // --- E. STUDIO MONITOR SPEAKERS (Left and Right of Monitors) ---
    // Left Studio Speaker
    obj.addBox(2.68f, -0.72f, -4.00f, 2.78f, -0.48f, -3.86f, uvSteel);
    obj.addQuad(Vec3(2.68f, -0.72f, -3.858f), Vec3(2.78f, -0.72f, -3.858f),
                Vec3(2.78f, -0.48f, -3.858f), Vec3(2.68f, -0.48f, -3.858f),
                Vec2(uvSpeaker.u0, uvSpeaker.v0), Vec2(uvSpeaker.u1, uvSpeaker.v0),
                Vec2(uvSpeaker.u1, uvSpeaker.v1), Vec2(uvSpeaker.u0, uvSpeaker.v1),
                Vec3(0, 0, 1.0f));

    // Right Studio Speaker
    obj.addBox(3.78f, -0.72f, -4.16f, 3.88f, -0.48f, -4.02f, uvSteel);
    obj.addQuad(Vec3(3.78f, -0.72f, -4.018f), Vec3(3.88f, -0.72f, -4.018f),
                Vec3(3.88f, -0.48f, -4.018f), Vec3(3.78f, -0.48f, -4.018f),
                Vec2(uvSpeaker.u0, uvSpeaker.v0), Vec2(uvSpeaker.u1, uvSpeaker.v0),
                Vec2(uvSpeaker.u1, uvSpeaker.v1), Vec2(uvSpeaker.u0, uvSpeaker.v1),
                Vec3(0, 0, 1.0f));

    // --- F. INPUT PERIPHERALS: KEYBOARD, MOUSE, DESK MAT, STREAM DECK ---
    // Oversized Stitched Desk Mat
    obj.addQuad(Vec3(2.92f, -0.718f, -3.48f), Vec3(3.88f, -0.718f, -3.48f),
                Vec3(3.88f, -0.718f, -3.85f), Vec3(2.92f, -0.718f, -3.85f),
                Vec2(uvRug.u0, uvRug.v0), Vec2(uvRug.u1, uvRug.v0),
                Vec2(uvRug.u1, uvRug.v1), Vec2(uvRug.u0, uvRug.v1),
                Vec3(0, 1.0f, 0));

    // Custom 75% Mechanical Keyboard (Wedge Incline)
    float kbMinX = 3.05f, kbMaxX = 3.52f;
    float kbFrontZ = -3.55f, kbRearZ = -3.75f;
    float kbFrontY = -0.715f, kbRearY = -0.698f;

    // Keyboard Base
    obj.addBox(kbMinX, -0.718f, kbRearZ, kbMaxX, kbFrontY, kbFrontZ, uvSteel);
    // Keyboard Keycap Top Face (Angled)
    obj.addQuad(Vec3(kbMinX, kbFrontY, kbFrontZ), Vec3(kbMaxX, kbFrontY, kbFrontZ),
                Vec3(kbMaxX, kbRearY,  kbRearZ),  Vec3(kbMinX, kbRearY,  kbRearZ),
                Vec2(uvKeyboard.u0, uvKeyboard.v0), Vec2(uvKeyboard.u1, uvKeyboard.v0),
                Vec2(uvKeyboard.u1, uvKeyboard.v1), Vec2(uvKeyboard.u0, uvKeyboard.v1),
                Vec3(0, 1.0f, 0));

    // Coiled Aviator Cable (Spiral segment + straight line to PC)
    obj.addBox(3.26f, -0.715f, -3.88f, 3.30f, -0.705f, -3.75f, uvSteel); // Straight
    obj.addBox(3.24f, -0.715f, -3.92f, 3.32f, -0.700f, -3.88f, uvSteel); // Quick-release metal aviator
    obj.addBox(3.32f, -0.715f, -3.95f, 3.92f, -0.708f, -3.92f, uvSteel); // Cable to PC Case!

    // Ergonomic Gaming Mouse
    float mX = 3.68f, mZ = -3.65f;
    obj.addBox(mX - 0.045f, -0.715f, mZ - 0.065f, mX + 0.045f, -0.688f, mZ + 0.065f, uvSteel);

    // Stream Deck (Macro Pad) on Left of Keyboard
    obj.addBox(2.93f, -0.715f, -3.76f, 3.03f, -0.685f, -3.64f, uvSteel);

    // --- G. HEADPHONE STAND & STUDIO HEADPHONES ---
    float hpX = 2.65f, hpZ = -3.62f;
    // Circular base
    obj.addCylinder(hpX, hpZ, -0.72f, -0.708f, 0.055f, 10, uvSteel);
    // Vertical rod
    obj.addCylinder(hpX, hpZ, -0.708f, -0.38f, 0.008f, 8, uvSteel);
    // Hanger hook
    obj.addBox(hpX - 0.01f, -0.38f, hpZ - 0.04f, hpX + 0.01f, -0.365f, hpZ + 0.04f, uvSteel);
    // Over-Ear Studio Headphones
    obj.addBox(hpX - 0.025f, -0.48f, hpZ - 0.035f, hpX - 0.005f, -0.40f, hpZ + 0.035f, uvSpeaker); // Left earcup
    obj.addBox(hpX + 0.005f, -0.48f, hpZ - 0.035f, hpX + 0.025f, -0.40f, hpZ + 0.035f, uvSpeaker); // Right earcup
    obj.addBox(hpX - 0.02f, -0.40f, hpZ - 0.02f,  hpX + 0.02f, -0.37f, hpZ + 0.02f, uvSteel); // Headband

    // Coffee Mug & Pen Holder
    obj.addCylinder(2.82f, -3.50f, -0.72f, -0.62f, 0.045f, 12, uvDrywall);
    obj.addCylinder(4.20f, -3.55f, -0.72f, -0.60f, 0.038f, 10, uvSteel);

    // 9. Executive Office Chair (Behind the Desk)
    float chairX = 3.35f, chairZ = -4.55f;
    obj.addCylinder(chairX, chairZ, -1.50f, -1.45f, 0.32f, 8, uvSteel);
    obj.addCylinder(chairX, chairZ, -1.45f, -1.05f, 0.035f, 8, uvSteel);
    obj.addBox(chairX - 0.26f, -1.05f, chairZ - 0.26f, chairX + 0.26f, -0.97f, chairZ + 0.26f, uvRug);
    obj.addBox(chairX - 0.24f, -0.97f, chairZ - 0.28f, chairX + 0.24f, -0.35f, chairZ - 0.22f, uvRug);
    obj.addBox(chairX - 0.16f, -0.35f, chairZ - 0.27f, chairX + 0.16f, -0.22f, chairZ - 0.23f, uvRug);
    obj.addBox(chairX - 0.29f, -0.90f, chairZ - 0.15f, chairX - 0.25f, -0.72f, chairZ + 0.15f, uvSteel);
    obj.addBox(chairX + 0.25f, -0.90f, chairZ - 0.15f, chairX + 0.29f, -0.72f, chairZ + 0.15f, uvSteel);

    // 10. Office Carpet Rug (Under Desk & Chair)
    obj.addQuad(Vec3(2.4f, -1.498f, -3.2f), Vec3(4.35f, -1.498f, -3.2f),
                Vec3(4.35f, -1.498f, -4.8f), Vec3(2.4f, -1.498f, -4.8f),
                Vec2(uvRug.u0, uvRug.v0), Vec2(uvRug.u1, uvRug.v0),
                Vec2(uvRug.u1, uvRug.v1), Vec2(uvRug.u0, uvRug.v1),
                Vec3(0, 1.0f, 0));

    // 11. Multi-Tier Bookshelf (Left Back Corner)
    float bsMinX = -2.35f, bsMaxX = -1.1f;
    float bsMinZ = -4.95f, bsMaxZ = -4.45f;
    obj.addBox(bsMinX, -1.5f, bsMinZ, bsMinX + 0.05f, 1.35f, bsMaxZ, uvDeskWood);
    obj.addBox(bsMaxX - 0.05f, -1.5f, bsMinZ, bsMaxX, 1.35f, bsMaxZ, uvDeskWood);
    obj.addBox(bsMinX, 1.30f, bsMinZ, bsMaxX, 1.35f, bsMaxZ, uvDeskWood);
    obj.addBox(bsMinX, -1.5f, bsMinZ, bsMaxX, 1.35f, bsMinZ + 0.03f, uvDeskWood);
    float shelfHeights[] = { -1.45f, -0.85f, -0.25f, 0.35f, 0.95f };
    for (float sy : shelfHeights) {
        obj.addBox(bsMinX + 0.05f, sy, bsMinZ + 0.03f, bsMaxX - 0.05f, sy + 0.04f, bsMaxZ, uvDeskWood);
    }
    obj.addBox(bsMinX + 0.08f, -0.81f, bsMinZ + 0.06f, bsMaxX - 0.15f, -0.32f, bsMaxZ - 0.05f, uvBooks);
    obj.addBox(bsMinX + 0.15f, -0.21f, bsMinZ + 0.06f, bsMaxX - 0.25f,  0.28f, bsMaxZ - 0.05f, uvBooks);
    obj.addCylinder(-1.72f, -4.68f, 0.39f, 0.44f, 0.08f, 10, uvPoster);
    obj.addCylinder(-1.72f, -4.68f, 0.44f, 0.70f, 0.05f, 10, uvPoster);

    // 12. Potted Office Plant (Beside the Window)
    float plantX = -1.95f, plantZ = -2.00f;
    obj.addCylinder(plantX, plantZ, -1.50f, -0.95f, 0.22f, 16, uvDrywall);
    obj.addCylinder(plantX, plantZ, -0.96f, -0.94f, 0.20f, 16, uvDeskWood);
    obj.addBox(plantX - 0.28f, -0.94f, plantZ - 0.28f, plantX + 0.28f, -0.25f, plantZ + 0.28f, uvFoliage);
    obj.addBox(plantX - 0.18f, -0.25f, plantZ - 0.18f, plantX + 0.18f,  0.15f, plantZ + 0.18f, uvFoliage);

    // 13. Floor Hazard / Caution Strip (In front of AI workers)
    obj.addQuad(Vec3(-1.6f, -1.497f, -1.80f), Vec3( 1.6f, -1.497f, -1.80f),
                Vec3( 1.6f, -1.497f, -1.95f), Vec3(-1.6f, -1.497f, -1.95f),
                Vec2(uvCaution.u0, uvCaution.v0), Vec2(uvCaution.u1, uvCaution.v0),
                Vec2(uvCaution.u1, uvCaution.v1), Vec2(uvCaution.u0, uvCaution.v1),
                Vec3(0, 1.0f, 0));

    // 14. Wastebasket next to manager desk
    obj.addCylinder(2.52f, -3.55f, -1.5f, -1.15f, 0.12f, 12, uvSteel);

    obj.saveOBJ("Models/scene_room.obj");
    std::cout << "[generate_office] Saved Models/scene_room.obj (" << obj.vertices.size() << " vertices, " << obj.faces.size() << " faces)\n";
    std::cout << "[generate_office] Done!\n";
    return 0;
}
