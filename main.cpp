#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <cstdlib>
using namespace std;

class Unit {
protected:
    string name;
    int hp;
    int mp;  // 能量
public:
    Unit(string n, int h, int m) : name(n), hp(h), mp(m) {}  // constructor

    virtual ~Unit() {}  // destructor
    // 只要你的類別會被當作「父類別」來繼承，而且會有 virtual 函式，那你的解構子就「一定要」是 virtual。

    virtual void attack(Unit *target) = 0;  // 參數要傳入一個指標，指向被攻擊的單位
    virtual void skill(Unit *target) = 0;
    void introduce() {
        cout << "[" << name << "] HP: " << hp <<  " MP: " << mp << endl;
    }
    virtual void takeDamage(int dmg) {
        hp = hp - dmg;
        if (hp<0) hp=0;
        cout << name << "受到了" << dmg << "點傷害";
        cout << " (剩餘 HP: " << hp << ")" << endl;
    }
    bool isDead() {
        if (hp<=0) return true;
        else return false;
    }
    int gethp() {
        return hp;
    }
    int getmp() {
        return mp;
    }
};

class Hero : public Unit{
public:
    Hero(string n, int h) : Unit(n, h, 30) {}
    void attack(Unit *target) override {
        // rand() % (最大值 - 最小值 + 1) + 最小值
        int dmg_hero = rand()%11+10;

        if (rand()%100<20) {
            dmg_hero = dmg_hero*2;
            cout << name << " 使出了暴擊！！！ (傷害翻倍)" << endl;
        }
        else {
            cout << name << " 揮劍攻擊！" << endl;
        }
        target->takeDamage(dmg_hero);
    }
    void skill(Unit *target) override {
        int cost = 10;
        if (mp<cost) {
            cout << name << " MP 不足！無法施放技能！" << endl;
            return;
        }
        mp = mp-cost;
        hp += 5;
        cout << name << " 恢復5點 HP。目前 HP: " << hp << endl;
    }
};

class Mage : public Unit {
public:
    Mage(string n, int h) : Unit(n,h,100) {}
    void attack(Unit *target) override {
        int dmg_mage = rand()%11+10;
        cout << name << " 使用法杖！" << endl;
        target->takeDamage(dmg_mage);
    }
    void skill(Unit *target) override {
        int cost = 25;
        if (mp<cost) {
            cout << name << " MP 不足！無法施放技能！" << endl;
            return;
        }
        mp = mp-cost;
        int dmg_mage = rand()%11+20;
        cout << name << "施放火球術！" << endl;
        target->takeDamage(dmg_mage);
    }
};

class Monster : public Unit {
public:
    Monster(string n, int h) : Unit(n, h, 0) {}
    // 這是打別人的時候
    void attack(Unit *target) override {
        int dmg_monster = rand()%11+5;
        cout << name << " 噴出火焰攻擊！" << endl;
        target->takeDamage(dmg_monster);
    }
    // 這是被打的時候
    void takeDamage(int dmg) override {
        if (rand()%100<30) {
            cout << name << " 閃過了攻擊！" << endl;
            return;
        }
        else Unit::takeDamage(dmg);  // 呼叫父類別已經寫好的功能
    }
    void skill(Unit *target) override {}
    // 不能省略，因為在父類別中是純虛擬函式
};

int main() {
    srand(time(nullptr));
    unique_ptr<Monster> monster = make_unique<Monster>("哥布林", 50);

    int role_choice;
    cout << "請選擇你的職業 (1.亞瑟/勇者  2.梅林/法師): ";
    cin >> role_choice;
    // 宣告一個「通用」的指標 (Unit*)
    unique_ptr<Unit> player;
    if (role_choice==1) {
        player = make_unique<Hero>("亞瑟", 100);
    }
    else if (role_choice==2) {
        player = make_unique<Mage>("梅林", 60);
    }
    else return 0;

    while (!player->isDead() && !monster->isDead()) {
        cout << "--------------------------------" << endl;
        player->introduce();
        monster->introduce();
        int choice;
        cout << "請選擇行動 (1.攻擊 2.特殊技能): ";
        cin >> choice;
        if (choice==1) {
            player->attack(monster.get());
        }
        else if (choice==2) {
            player->skill(monster.get());
        }
        else cout << "亞瑟什麼都沒做，發呆了一回合..." << endl;

        // 怪物回合
        if (!monster->isDead()) {
            monster->attack(player.get());
        }
        cout << "--------------------------------" << endl;
    }
    if (player->isDead()) {
    cout << "你輸了... Game Over." << endl;
    } 
    else cout << "勝利！怪物倒下了！" << endl;
    return 0;
}

// 解釋 : player->attack(monster.get());
// 不能寫 player->attack(monster); (一個是原始指標，一個是智慧指標)
// 意思是：取出 monster 裡面的那個 Unit* 原始指標，傳進去給 attack 用
// .get() = 取得內部的原始指標
