#include "Game.h"

#include <iostream>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <fstream>
#include <cmath>

Game::Game(const std::string &config) {
    init(config);
}

void Game::init(const std::string &path) {
    // sf::Vector2u size = m_window.getSize();
    // [m_wWidth, m_wHeight] = size;

    const std::string filePath = "../" + path;     
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file." << std::endl;
        return;
    }
    
    unsigned int frameLimit;
    bool isFullscreen = 0;


    std::string temp;
    while (file >> temp) {
        if (temp == "Window") {
            file >> m_wWidth >> m_wHeight >> frameLimit;
            file >> isFullscreen;
        }
        else if (temp == "Font") {
            file >> temp;
            if (!m_font.openFromFile("../" + temp))
            {
                std::cerr << "Could not open font file.\n";
                return;
            }
            file >> m_textSize;
            file >> m_textR >> m_textG >> m_textB;
        }
        else if (temp == "Player") {
            file >> m_playerConfig.SR >> m_playerConfig.CR;
            file >> m_playerConfig.S;
            file >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
            file >> m_playerConfig.SE >> m_playerConfig.SSE >> m_playerConfig.PD;
        }
        else if (temp == "Enemy") {
            file >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
        }
        else if (temp == "Bullet") {
            file >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
        }
    }

    // Close the file stream manually (or let the destructor handle it)
    file.close();

    // set up default window parameters
    m_window.create(sf::VideoMode({m_wWidth, m_wHeight}), "Assignment 2", isFullscreen);

    // sf::RenderWindow m_window(sf::VideoMode({1280, 720}), "Assignment 2");
    m_window.setFramerateLimit(frameLimit); // limit frame rate to 60 fps

    // initialize IMGUI and create a clock used for its internal timing
    if (!ImGui::SFML::Init(m_window))
    {
        std::cerr << "Failed to initialize ImGui-SFML\n";
        return;
    }

    // // scale the imgui ui and text size by 2
    // ImGui::GetStyle().ScaleAllSizes(2.0f);
    // ImGui::GetIO().FontGlobalScale = 2.0f;

    spawnPlayer();
    // spawnEnemy();
}

void Game::run() {
    // TODO:
    // - add pause functionality in here
    // - some systems should function while paused (rendering)
    // - some systems shouldn't (movement / input)
    
    // Seed the generator with the current time so it changes every run
    std::srand(std::time(nullptr)); 

    while (m_running)
    {
        // update the entity manager
        m_entitiesToAdd.update();
        m_entities.update();



        // required update call to imgui
        // ImGui::SFML::Update(m_window, m_deltaClock.restart());

        if (m_isSpawningActive) {
            sEnemySpawner();
        }
        if (m_isMovementActive) {
            sMovement();
        }

        sGUI();
        
        if (m_isCollisionActive) {
            sCollision();
        }
        sUserInput();
        if (m_isLifespanActive) {
            sLifespan();
        }
        sRender();

        // increment the current frame
        // may need to be moved when pause implemented
        m_currentFrame++;
    }
    ImGui::SFML::Shutdown();
}

void Game::setPaused(bool paused) {
    // TODO
}

// respawn the player in the middle of the screen
void Game::spawnPlayer() {
    // We create every entity by calling EntityManager.addEntity(tag)
    // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
    auto entity = m_entities.addEntity("player");

    // Give this entity a Transform, so it spawns at (200,200) with velocity (1,1) and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vec2(m_wWidth / 2.f, m_wHeight / 2.f), Vec2(m_playerConfig.S, m_playerConfig.S), 0.0f);

    // The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness 4
    // entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
    // Add an input component to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // Since we want this Entity to be our player, set our Game's player variable to be this Entity
    // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy() {
    // TODO:
    // make sure the enemy is spawned properly with the m_enemyConfig variables
    // the enemy must be spawned completely within the bounds of the window

    auto e = m_entities.addEntity("enemy");

    const int SCREEN_OFFSET = 100;
    const int MAX_POINT_COUNT = 8;
    int randomX = (std::rand() % (1280 - SCREEN_OFFSET)) + SCREEN_OFFSET; 
    int randomY = (std::rand() % (720 - SCREEN_OFFSET)) + SCREEN_OFFSET; 
    int randPointCount = (std::rand() % m_enemyConfig.VMAX) + m_enemyConfig.VMIN;
    float minSpeed = m_enemyConfig.SMIN;
    float maxSpeed = m_enemyConfig.SMAX;
    float randSpeed = (std::rand() % (int)minSpeed) + maxSpeed;

    sf::Color enemyColor = sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);
    // apply random color to each enemy
    sf::Color randEnemyColor = sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256);

    e->cTransform = std::make_shared<CTransform>(Vec2(randomX, randomY), Vec2(randSpeed, randSpeed), 0.0f);
    e->cShape = std::make_shared<CShape>(m_enemyConfig.SR, randPointCount, randEnemyColor, enemyColor, m_enemyConfig.OT);
    e->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e) {
    // TODO: spawn small enemies at the location of the input enemy e

    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the vertices of the original enemy
    // - set each small enemy to the same color as the original, half the size
    // - small enemies are worth double points of the original enemy

    int n = e->cShape->circle.getPointCount();
    std::vector<std::pair<float, float> > direction = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    const float pi = 3.141592; 
    
    for (size_t i = 0; i < n; i++) {
        float angle = (360.f / n) * i;
        float theta = angle * (pi / 180.0f);
        // Compute the sine value
        float resultX = std::cos(theta);
        float resultY = std::sin(theta);

        auto smallE = m_entities.addEntity("smallEnemy");
        smallE->cTransform = std::make_shared<CTransform>(e->cTransform->pos, Vec2(resultX, resultY) * 3.f, 0.0f);
        smallE->cShape = std::make_shared<CShape>(m_enemyConfig.SR / 2.f, n, e->cShape->circle.getFillColor(), e->cShape->circle.getOutlineColor(), m_enemyConfig.OT);
        smallE->cCollision = std::make_shared<CCollision>(e->cCollision->radius);
        smallE->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target) {
    // TODO: implement the spawning of a bullet which travels toward target
    // - bullet speed is given as a scalar speed
    // - you must set the velocity by using formula in notes
    const int BULLET_SPEED = m_bulletConfig.S;
    auto bullet = m_entities.addEntity("bullet");
    bullet->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos, Vec2(3, 3), 0.f);
    bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), 0.0f); 
    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);

    // originate the bullet from the center of entity and move towards target
    Vec2 normalizedVelocity = target;
    normalizedVelocity -= bullet->cTransform->pos;
    normalizedVelocity /= normalizedVelocity.length();
    // std::cerr << "normalizedVelocity: " << normalizedVelocity.x << " " << normalizedVelocity.y << '\n';

    bullet->cTransform->velocity.x = BULLET_SPEED * normalizedVelocity.x;
    bullet->cTransform->velocity.y = BULLET_SPEED * normalizedVelocity.y;
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
    // TODO: implement your own special weapon
}

void Game::sMovement() {
    // TODO: implement all entity movement in this function
    // you should read the m_player->cInput component to determine if the player is moving
    // sample:

    // player movement speed update
    int inputCnt = m_player->cInput->up + m_player->cInput->down + m_player->cInput->left + m_player->cInput->right;
    if (inputCnt > 1) {
        m_player->cTransform->velocity = Vec2(0.707f, 0.707f) * m_playerConfig.S;
    }
    else {
        m_player->cTransform->velocity = Vec2(1.f, 1.f) * m_playerConfig.S;
    }
    if (m_player->cInput->up) {
        m_player->cTransform->pos.y -= std::abs(m_player->cTransform->velocity.y);
        m_player->cInput->up = false;
    }
    if (m_player->cInput->down) {
        m_player->cTransform->pos.y += std::abs(m_player->cTransform->velocity.y);
        m_player->cInput->down = false;
    }
    if (m_player->cInput->left) {
        m_player->cTransform->pos.x -= std::abs(m_player->cTransform->velocity.x);
        m_player->cInput->left = false;
    }
    if (m_player->cInput->right) {
        m_player->cTransform->pos.x += std::abs(m_player->cTransform->velocity.x);
        m_player->cInput->right = false;
    }
    
    // set the position of the shape based on the entity's transform->pos
    m_player->cShape->circle.setPosition({m_player->cTransform->pos.x, m_player->cTransform->pos.y});

    // set the rotation of the shape based on the entity's transform->angle
    m_player->cTransform->angle += 3.0f;
    m_player->cShape->circle.setRotation(sf::degrees(m_player->cTransform->angle));

    // Move all enemies
    auto enemyEntities = m_entities.getEntities("enemy");
    for (auto e: enemyEntities)
    {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;

        e->cShape->circle.setPosition({e->cTransform->pos.x, e->cTransform->pos.y});

        // set the rotation of the shape based on the entity's transform->angle
        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(sf::degrees(e->cTransform->angle));
    }

    // Move small enemies
    for (auto e : m_entities.getEntities("smallEnemy")) {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;

        e->cShape->circle.setPosition({e->cTransform->pos.x, e->cTransform->pos.y});

        // set the rotation of the shape based on the entity's transform->angle
        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(sf::degrees(e->cTransform->angle));
    }

    // bullet movement
    for (auto e : m_entities.getEntities("bullet")) {
        // std::cerr << "Bullet pos: " << e->cTransform->pos.x << ' ' << e->cTransform->pos.y << '\n';
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;

        e->cShape->circle.setPosition({e->cTransform->pos.x, e->cTransform->pos.y});
    }

    // remove any entities which is out of screen
    for (auto e : m_entities.getEntities()) {
        if (e->cTransform->pos.x > m_wWidth * 2.f || e->cTransform->pos.x < -2.f * m_wWidth) {
            e->destroy();
        }
        if (e->cTransform->pos.y > m_wHeight * 2.f || e->cTransform->pos.y < -2.f * m_wHeight) {
            e->destroy();
        }
    }
}

void Game::sLifespan() {
    // TODO: implement all lifespan functionality
    //
    // for all entities
    // - if entity has no lifespan component, skip it
    // - if entity has > 0 remaining lifespan, subtract 1
    // - if it has lifespan and is alive scale its alpha channel properly
    // - if it has lifespan and its time is up destroy the entity


    for (auto e : m_entities.getEntities()) {
        if (!(e->tag() == "bullet" || e->tag() == "smallEnemy")) {
            continue;
        }
        e->cLifespan->remaining -= 1;
        if (e->cLifespan->remaining <= 0) {
            e->destroy();
        }
        else {
            sf::Color fill_color = e->cShape->circle.getFillColor();
            sf::Color outline_color = e->cShape->circle.getOutlineColor();
            fill_color.a = (1.f * e->cLifespan->remaining / e->cLifespan->total) * 255.f;
            outline_color.a = (1.f * e->cLifespan->remaining / e->cLifespan->total) * 255.f;

            // int alpha = (e->cLifespan->remaining / e->cLifespan->total) * 255.f;
            e->cShape->circle.setFillColor(fill_color);
            e->cShape->circle.setOutlineColor(outline_color);
        }
    }
}

void Game::sCollision() {
    // TODO: implement all proper collisions between entities
    // be sure to use the collision radius, not the shape radius

    // collision check between player and enemies
    for (auto enemy: m_entities.getEntities("enemy")) {
        if (enemy->isActive()) {
            // dist. between the centers of the 2 circles
            float centerDist = m_player->cTransform->pos.distSquare(enemy->cTransform->pos);
            // std::cerr << "Center dist: " << centerDist << '\n';
            float radiusSum = m_player->cCollision->radius + enemy->cCollision->radius;
            if (centerDist < (radiusSum * radiusSum)) {
                // destroy enemy and spawn small enemies
                std::cerr << "Player collided with Enemy " << enemy->id() << '\n';
                // re-spawn palyer at middle of screen                
                m_player->cTransform->pos = Vec2({m_wWidth / 2.f, m_wHeight / 2.f});

                enemy->destroy();
                m_score -= m_playerConfig.PD;
            }
        }
    }

    // collision check between player and small Enemies
    for (auto enemy: m_entities.getEntities("smallEnemy")) {
        if (enemy->isActive()) {
            // dist. between the centers of the 2 circles
            float centerDist = m_player->cTransform->pos.distSquare(enemy->cTransform->pos);
            // std::cerr << "Center dist: " << centerDist << '\n';
            float radiusSum = m_player->cCollision->radius + enemy->cCollision->radius;
            if (centerDist < (radiusSum * radiusSum)) {
                // destroy enemy and spawn small enemies
                std::cerr << "Player collided with Enemy " << enemy->id() << '\n';
                // re-spawn palyer at middle of screen                
                m_player->cTransform->pos = Vec2({m_wWidth / 2.f, m_wHeight / 2.f});

                enemy->destroy();
                m_score -= m_playerConfig.PD;
            }
        }
    }

    // collision check between bullet and enemy
    for (auto bullet: m_entities.getEntities("bullet")) {
        for (auto entities: m_entities.getEntities("enemy")) {
            if (bullet->isActive() && entities->isActive()) {
                // dist. between the centers of the 2 circles
                float centerDist = bullet->cTransform->pos.distSquare(entities->cTransform->pos);
                // std::cerr << "Center dist: " << centerDist << '\n';
                float radiusSum = m_bulletConfig.CR + entities->cCollision->radius;
                if (centerDist < (radiusSum * radiusSum)) {
                    // destroy enemy and spawn small enemies
                    std::cerr << "Bullet " << bullet->id() << " collided with Enemy " << entities->id() << '\n';
                    bullet->destroy();
                    entities->destroy();
                    m_score += m_playerConfig.SE;
                    spawnSmallEnemies(entities);
                }
            }
        }

        // we need another loop for small entities because small entities don't spawn further
        for (auto entities: m_entities.getEntities("smallEnemy")) {
            if (bullet->isActive() && entities->isActive()) {
                // dist. between the centers of the 2 circles
                float centerDist = bullet->cTransform->pos.distSquare(entities->cTransform->pos);
                // std::cerr << "Center dist: " << centerDist << '\n';
                float radiusSum = m_bulletConfig.CR + entities->cCollision->radius;
                if (centerDist < (radiusSum * radiusSum)) {
                    // destroy enemy and spawn small enemies
                    std::cerr << "Bullet " << bullet->id() << " collided with Small enemy " << entities->id() << '\n';
                    bullet->destroy();
                    entities->destroy();
                    m_score += m_playerConfig.SSE;
                }
            }
        }
    }

    // entities colliding with walls
    auto allEntities = m_entities.getEntities();
    for (auto e: allEntities)
    {   
        // if e->hasCollison then proceed otherwise continue to next entity
        if (!(e->tag() == "enemy" || e->tag() == "smallEnemy" || e->tag() == "player")) {
            continue;
        }

        // if you want to allow player to teleport to the other side of map when touching wall, just toggle std::max and std::min
        if (e->tag() == "player") {
            if (e->cTransform->pos.x - e->cCollision->radius < 0) {
                e->cTransform->pos.x += std::max(0.f, e->cTransform->velocity.x);
            }    
            if (e->cTransform->pos.x + e->cCollision->radius > (float)m_wWidth) {
                e->cTransform->pos.x -= std::min((float)m_wWidth - e->cCollision->radius, e->cTransform->velocity.x);
            }
            if (e->cTransform->pos.y - e->cCollision->radius < 0) {
                e->cTransform->pos.y += std::max(0.f, e->cTransform->velocity.y);
            }        
            if (e->cTransform->pos.y + e->cCollision->radius > (float)m_wHeight) {
                e->cTransform->pos.y -= std::min((float)m_wHeight - e->cCollision->radius, e->cTransform->velocity.y);
            }
            continue;
        }
        if (e->cTransform->pos.x - e->cCollision->radius < 0 || e->cTransform->pos.x + e->cCollision->radius > (float)m_wWidth) {
            e->cTransform->velocity.x *= -1;
        }
        if (e->cTransform->pos.y - e->cCollision->radius < 0 || e->cTransform->pos.y + e->cCollision->radius > (float)m_wHeight) {
            e->cTransform->velocity.y *= -1;
        }

    }
}

void Game::sEnemySpawner() {
    // TODO: code which implements enemy spawning should go here
    if (m_currentFrame - m_lastEnemySpawnTime > m_enemyConfig.SI) {
        spawnEnemy();
    }
}

void Game::sGUI() {
    ImGui::SFML::Update(m_window, m_deltaClock.restart());
    ImGui::Begin("Geometry Wars");

    // Create a unique identifier for the tab bar system
    if (ImGui::BeginTabBar("Tab1")) 
    {
        // First Tab
        if (ImGui::BeginTabItem("Systems")) 
        {
            // Movement

            // The macro/function takes a label and a pointer to your bool
            if (ImGui::Checkbox("Movement", &m_isMovementActive)) 
            {
                // This block runs ONLY on the exact frame the user clicks the checkbox
                std::cerr << "Checkbox toggled! New state: " << m_isMovementActive << "\n";
            }

            // Collision
            if (ImGui::Checkbox("Collision", &m_isCollisionActive)) 
            {
                std::cerr << "Checkbox toggled! New state: " << m_isCollisionActive << "\n";
            }
            
            // Spawning
            if (ImGui::Checkbox("Spawning", &m_isSpawningActive)) 
            {
                std::cerr << "Checkbox toggled! New state: " << m_isSpawningActive << "\n";
            }

            // Spawn interval
            // Render the integer slider inside your ImGui window loop
            if (ImGui::SliderInt("Spawn Interval", &m_enemyConfig.SI, 40, 120)) 
            {
                // This code runs ONLY on the frame the user changes the slider value
                std::cerr << "Spawn interval changed to: " << m_enemyConfig.SI << '\n';
            }

            // Manual Spawn
            if (ImGui::Button("Manual Spawn")) 
            {
                // This code executes only on the frame the user clicks the button
                spawnEnemy();
            }

            ImGui::EndTabItem();
        }

        // Second Tab
        if (ImGui::BeginTabItem("Entity Manager")) 
        {
            // ImGui::Text("Entities");
            // Entities by Tag
            if (ImGui::CollapsingHeader("Entities by Tag")) 
            {
                const char* allTags[] = {"player", "smallEnemy", "enemy", "bullet"};
                for (size_t i = 0; i < sizeof(allTags) / sizeof(allTags[0]); i++) { 
                    if (ImGui::CollapsingHeader(allTags[i])) 
                    {
                        for (auto e : m_entities.getEntities(allTags[i])) {
                            // Dynamically builds "D##1", "D##2", etc.
                            std::string buttonId = std::format("Delete {}{}", allTags[i], e->id());  
                            if (e->isActive() && ImGui::Button(buttonId.c_str())) 
                            {
                                e->destroy();
                            }
                            ImGui::SameLine();
                            std::string posStr = "( " + std::to_string(e->cTransform->pos.x) + ", " + std::to_string(e->cTransform->pos.y) + ")";
                            ImGui::Text(posStr.c_str());
                        }
                    }  
                } 
            }

            // All entities
            if (ImGui::CollapsingHeader("All entities")) 
            {
                // Content placed here only shows when the header is open
                for (auto e : m_entities.getEntities()) {
                    // Dynamically builds "D##1", "D##2", etc.
                    std::string buttonId = std::format("Delete {}{}", e->id(), e->id());  
                    if (e->isActive() && ImGui::Button(buttonId.c_str())) 
                    {
                        e->destroy();
                    }
                    ImGui::SameLine();
                    std::string posStr = "( " + std::to_string(e->cTransform->pos.x) + ", " + std::to_string(e->cTransform->pos.y) + ")";
                    ImGui::Text(posStr.c_str());
                }
            }
            ImGui::EndTabItem();
        }

        // Always match BeginTabBar with EndTabBar
        ImGui::EndTabBar(); 
    }

    ImGui::End();
}

void Game::sRender() {
    m_window.clear();

    // draw the entity's sf::CircleShape
    m_window.draw(m_player->cShape->circle);
    // std::cerr << "Player id: " << m_player->id() << '\n';
    
    // draw all active entities
    for (auto e : m_entities.getEntities()) {
        if (e->isActive() == false) {
            continue;
        }
        m_window.draw(e->cShape->circle);
    }
    // score text
    sf::Text scoreText(m_font);
    std::string scoreStr = std::to_string(m_score);
    scoreText.setString("Score: " + scoreStr);
    scoreText.setCharacterSize(m_textSize); // in pixels, not points!
    scoreText.setFillColor(sf::Color(m_textR, m_textG, m_textB));
    
    sf::Text isPausedText(m_font);
    isPausedText.setPosition({m_wWidth - 110.f, 8.f});
    isPausedText.setString("Paused");
    isPausedText.setCharacterSize(m_textSize); // in pixels, not points!
    isPausedText.setFillColor(sf::Color(m_textR, m_textG, m_textB));

    m_window.draw(scoreText);
    if (m_pause) {
        m_window.draw(isPausedText);
    }

    // draw the ui last
    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::sUserInput() {
    // TODO: handle user input here
    // note that you should only be setting the player's input component variables here
    // you shold not implement the player's movement logic here
    // the movement system will read the variables you set in this functioin

    // EVENT POLLING (One-time actions & state changes)
    while (const std::optional event = m_window.pollEvent())
    {
        // Allow ImGui to listen to events
        ImGui::SFML::ProcessEvent(m_window, *event);

        if (event->is<sf::Event::Closed>())
        {
            std::cerr << "Window Closed\n";
            m_window.close();
            m_running = false;
        }

        // Key Press Events
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            switch (keyPressed->code)
            {
                case sf::Keyboard::Key::E:
                    spawnEnemy();
                    break;

                case sf::Keyboard::Key::Escape:
                    m_window.close();
                    m_running = false;
                    break;

                case sf::Keyboard::Key::P:
                    // pause game
                    m_pause = !m_pause;
                    m_isMovementActive = !m_isMovementActive;
                    m_isSpawningActive = !m_isSpawningActive;
                    m_isLifespanActive = !m_isLifespanActive;
                    break;

                default:
                    break;
            }
        }

        // Mouse Events
        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (ImGui::GetIO().WantCaptureMouse) { continue; }

            if (mousePressed->button == sf::Mouse::Button::Left)
            {
                sf::Vector2 sfMousePos = sf::Mouse::getPosition(m_window);
                Vec2 mousePos = Vec2(sfMousePos.x, sfMousePos.y);
                spawnBullet(m_player, mousePos);
            }
        }
    }

    // Reset flags every frame so movement stops when keys are released
    m_player->cInput->up    = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
    m_player->cInput->down  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
    m_player->cInput->left  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    m_player->cInput->right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
}


// void collisions()
// {
// for (auto b : m_entities.getEntities("bullet"))
// for (auto e : m_entities.getEntities("enemy"))
//     if (Physics::CheckCollision(b, e))
//     {
//     b->destroy();
//     e->destroy();
//     }
// }
