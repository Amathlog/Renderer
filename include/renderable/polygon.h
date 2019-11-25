#include <renderable/renderable.h>
#include <vector>

class Polygon : public Renderable
{
public:
    Polygon(const std::vector<float>& vertices, 
            const std::vector<unsigned int>& indexes, 
            const glm::vec4& color,
            Shader* specificShader = nullptr);
    
    virtual ~Polygon();

    virtual void CreateShader() override;

    glm::vec4& GetColor() {return m_color;}
    const glm::vec4& GetColor() const {return m_color;}

protected:
    virtual void InternalDraw(const glm::mat4& mvp) const override;
    
    unsigned int m_VBO;
    unsigned int m_VAO;
    unsigned int m_EBO;
    glm::vec4 m_color;
    unsigned int m_nbVertices;
};