
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <map>
#include <memory>
#include <set>


namespace Game {

class Bet {
private:
    int amount;
    std::string player_name;

public:
    Bet(int amount, const std::string &player_name)
        : amount(amount), player_name(player_name) {}

    int get_amount() const { return amount; }
    const std::string &get_player_name() const { return player_name; }
};


class Pot {
private:
    int pot{0};  // Total chips in the pot
    std::vector<std::shared_ptr<Bet>> bets;  // Collection of bets made

public:
    Pot() = default;

    // Adds a bet to the pot
    void add_to_pot(const std::shared_ptr<Bet> &bet) {
        std::cout << "Player " << bet->get_player_name() << " added "
                  << bet->get_amount() << " chips to the pot." << std::endl;
        pot += bet->get_amount();
        bets.push_back(bet);
    }
    
    // Resets the pot to 0
    void reset(){
        pot = 0;
    }

    // Returns the current value of the pot
    int get_final_pot() { return pot; }

    // Returns the collection of bets made
    const std::vector<std::shared_ptr<Bet>> &get_bets() const { return bets; }
};

} // Game namespace end



namespace Cards {

class Card {
private:
    std::string suit;  // Card suit (e.g., hearts, diamonds, clubs, spades)
    std::string rank;  // Card rank (e.g., 2, 3, 4, ..., J, Q, K, A)

public:
    Card(std::string i_suit, std::string i_rank)
        : suit(i_suit), rank(i_rank) {}

    std::string get_suit() const { return suit; }  // Get the card suit
    std::string get_rank() const { return rank; }  // Get the card rank
};

std::ostream& operator<<(std::ostream& os, const Card& card) {
    os << card.get_rank() << card.get_suit();
    return os;
}

class Card_Container {
protected:
    std::vector<Card> cards;  // Collection of cards
    std::vector<std::string> suits = { "♥", "♦", "♣", "♠" };  // Available suits
    std::vector<std::string> ranks = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };  // Available ranks

public:
    Card_Container() {
        recreate();
    }

    void print_cards() const {
        for (const auto& card : cards) {
            std::cout << card.get_rank() << card.get_suit() << " ";
        }
        std::cout << std::endl;
    }
    
    void recreate() {
        cards.clear();

        for (const auto& suit : suits) {
            for (const auto& rank : ranks) {
                cards.emplace_back(suit, rank);
            }
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }

    virtual void reset() = 0;  // Virtual function to reset the card container
};




class Deck : public Card_Container {
protected:
    std::vector<Card> game_cards;        // Cards used in the game
    std::vector<Card> community_cards;   // Community cards

public:
    int nr_of_community_cards{};         // Number of community cards
    int nr_of_players{};                 // Number of players

    Deck(int i_nr_of_players, int i_nr_of_community_cards)
        : nr_of_players(i_nr_of_players),
          nr_of_community_cards(i_nr_of_community_cards) {
        populate_game_cards();
    }

    void populate_game_cards() {
        // Populate the game cards by taking cards from the card container
        for (int i{0}; i < nr_of_community_cards + nr_of_players * 2; i++) {
            Card selected_card = cards.back();
            cards.pop_back();
            game_cards.push_back(selected_card);
        }
    }

    Card take_game_card() {
        // Take a card from the game cards
        Card taken_card = game_cards.back();
        game_cards.pop_back();

        return taken_card;
    }

    bool flop_is_taken = false;
    void take_flop() {
        // Take the flop cards from the game cards and add them to the community cards
        if (flop_is_taken == false) {
            Card flop1 = take_game_card();
            Card flop2 = take_game_card();
            Card flop3 = take_game_card();
            community_cards.push_back(flop1);
            community_cards.push_back(flop2);
            community_cards.push_back(flop3);
            flop_is_taken = true;
        }
    }

    bool turn_is_taken = false;
    void take_turn() {
        // Take the turn card from the game cards and add it to the community cards
        if (turn_is_taken == false) {
            Card turn_card = take_game_card();
            community_cards.push_back(turn_card);
            turn_is_taken = true;
        }
    }

    bool river_is_taken = false;
    void take_river() {
        // Take the river card from the game cards and add it to the community cards
        if (river_is_taken == false) {
            Card river_card = take_game_card();
            community_cards.push_back(river_card);
            river_is_taken = true;
        }
    }

    void print_community_cards() {
        // Print the community cards
        std::cout << "The community cards: ";
        for (auto object : community_cards) {
            std::cout << object << " ";
        }
        std::cout << std::endl;
    }

    std::vector<Card> get_community_cards() {
        // Get a copy of the community cards
        std::vector<Card> local_container;
        for (auto object : community_cards) {
            local_container.push_back(object);
        }
        return local_container;
    }

    void reset() override {
        // Reset the card container and game-specific variables
        cards.clear();
        game_cards.clear();
        community_cards.clear();
        flop_is_taken = false;
        turn_is_taken = false;
        river_is_taken = false;
    }
};


} //namespace Cards end

namespace Players {

using Cards::Card;
using Cards::Deck;
using Game::Bet;
using Game::Pot;

class Player {
protected:
    std::string name;
    int chips;
    Card card1;
    Card card2;

public:
    Player(std::string i_name, int i_chips, Card i_card1, Card i_card2)
        : name(i_name),
          card1(i_card1),
          card2(i_card2),
          chips(i_chips) {}

    std::string get_name() const { return name; }
    int get_chips() const { return chips; }
    Card get_card1() const { return card1; }
    Card get_card2() const { return card2; }
    
    // Static function to show player information
    static void show_player_info(std::vector<Player> selected_players) {
        for (auto &object : selected_players) {
            std::cout << "Bot opponent: " << object.get_name() << ", chips: " << object.get_chips() << std::endl;
        }
    }
    
    bool operator==(const Player& other) const {
        // Compare the attributes of the Player class, for example:
        return name == other.name && chips == other.chips; // Add any other attributes you want to compare.
    }
    
    // Player class
    
    // Make a bet and add it to the pot
    std::shared_ptr<Bet> bet(int amount, Pot& pot) {
        std::string betting_player_name{ get_name() };
        std::shared_ptr<Bet> bet = std::make_shared<Bet>(amount, betting_player_name);
        pot.add_to_pot(bet);
        chips -= amount;
        
        return bet;
    }
    
    // Receive a share of the pot
    void receive_pot_share(int amount) {
        chips += amount;
    }
    
    // Set new cards for the player
    void set_new_cards(Card input_card1, Card input_card2) {
        card1 = input_card1;
        card2 = input_card2;
    }
    
    // Update the player's chips
    void update_chips(int new_chips) {
        chips = new_chips;
    }
};

class Bot : public Player {
public:
    Bot() : Player("", 0, Card("", ""), Card("", "")) {}

    std::vector<Player> bots;
    
    std::vector<std::string> names = { "Alice", "Bence", "Carol", "David", "Eve", "Frank", "Grace", "Helen", "Ivan", "Judy", "Karl", "Laura", "Mike", "Nancy", "Oscar", "Paul", "Quincy", "Rita", "Steve", "Tina" };
     
    // Get a bot name from the list
    std::string take_bot_name() {
        std::string bot_name = names.back();
        names.pop_back();
        return bot_name;
    }
    
    // Create bots with random cards
    void create_bots(Deck& deck, int nr_bots, int start_chips) {
        std::shuffle(names.begin(), names.end(), std::mt19937{std::random_device{}()});
        for (int i{0}; i < nr_bots; i++) {
            Card card1 = deck.take_game_card();
            Card card2 = deck.take_game_card();
            std::string bot_name = take_bot_name();
            bots.emplace_back(bot_name, start_chips, card1, card2);
        }
    }
    
    // Show information about the default bots
    void show_default_bots() {
        for (const auto& object : bots) {
            std::cout << "This is a default bot player in this game: " << object.get_name()
                      << ", cards: " << object.get_card1().get_rank() << object.get_card1().get_suit()
                      << " " << object.get_card2().get_rank() << object.get_card2().get_suit()
                      << ", chips: " << object.get_chips() << std::endl;
        }
    }
    
    // Get the bots as a vector of players
    std::vector<Player> get_bots() {
        std::vector<Player> local_container;
        for(auto& bot : bots) {
            local_container.push_back(bot);
        }
        return local_container;
    }
    
    // Get the number of bots
    int get_bots_number() {
        return bots.size();
    }
    
    // Redistribute new cards to the bots from the deck
    void redistribute_bot_cards(Deck& deck) {
        for (auto& bot : bots) {
            bot.set_new_cards(deck.take_game_card(), deck.take_game_card());
        }
    }

    // Convert bots to a vector of reference wrappers
    std::vector<std::reference_wrapper<Player>> refer_updated_bots() {
        std::vector<std::reference_wrapper<Player>> local_container;
        for(auto& bot : bots) {
            local_container.push_back(std::ref(bot));
        }
        return local_container;
    }
    
    void update_bots() {
        std::vector<std::reference_wrapper<Player>> botRefs = this->refer_updated_bots();
        bots.clear();
        for(auto& botRef : botRefs) {
            Player updatedBot = botRef.get();
            bots.push_back(updatedBot);
        }
    }

    void update_bot_chips(std::string bot_name, int amount){
        
        for (auto &object : bots){
            if (object.get_name() == bot_name){
                object.update_chips(object.get_chips() - amount);
                break;
            }
        }
    }
    
    void update_chips_for_bot(const std::string& bot_name, int amount) {
    for (auto& bot : bots){
        if (bot.get_name() == bot_name){
            bot.receive_pot_share(amount);
            break;
        }
    }
}

    void delete_defeated_bots() {
    for (auto it = bots.begin(); it != bots.end();) {
        if (it->get_chips() == 0) {
            it = bots.erase(it);  // Use the return value of erase to get a valid iterator
        } else {
            ++it;  // Only increment the iterator if we didn't erase an element
        }
    }
}



    
};

class Human : public Player {
public:
    
    // Constructor for the Human player
    Human(int start_chips, Deck& deck)
        : Player("Human", start_chips, deck.take_game_card(), deck.take_game_card()) {}

    // Show the human player's cards
    void show_human_cards() {
        Card c1 = get_card1();
        Card c2 = get_card2();
        std::cout << "Your cards are: " << c1.get_rank() << c1.get_suit() << " " << c2.get_rank() << c2.get_suit() << std::endl;
    }
    
    // Redistribute new cards to the human player from the deck
    void redistribute_human_cards(Deck& deck){
        this->set_new_cards(deck.take_game_card(), deck.take_game_card());
    }
};


} // namespace Players end

namespace Game {

using Cards::Deck;
using Cards::Card;
using namespace Players;


enum class Poker_Ranks {
    high_card = 0,
    pair = 1,
    two_pair = 2,
    three_of_a_kind = 3,
    straight = 4,
    flush = 5,
    full_house = 6,
    four_of_a_kind = 7,
    straight_flush = 8,
    royal_flush = 9
    
    
};







class Ranking {

private:
    static bool has_run;


public:
    

    static bool compare_hand_ranks(const std::pair<std::string, std::pair<Poker_Ranks, int>>& a, const std::pair<std::string, std::pair<Poker_Ranks, int>>& b) {
        if (a.second.first != b.second.first) {
            return a.second.first > b.second.first;
        }
        return a.second.second > b.second.second;
    }
    
    static void reset(){
        has_run=false;
    }
    

    // Determine the winner function
    std::vector<std::string> determine_winner(const std::vector<Player>& players, Deck& deck) {
        const std::vector<Card> community_cards = deck.get_community_cards();
        std::vector<std::pair<std::string, std::pair<Poker_Ranks, int>>> player_name_and_rank;

        for (const auto& player : players) {
            Card card1 = player.get_card1();
            Card card2 = player.get_card2();

            std::pair<Poker_Ranks, int> player_hand = evaluate_hand(card1, card2, community_cards);

            std::cout << "Final score:  player named: " << player.get_name() << " had these cards: "
                      << card1.get_rank() << card1.get_suit() << " " << card2.get_rank() << card2.get_suit()
                      << "  and the FINAL RANK: " << poker_rank_to_string(player_hand.first) << std::endl;

            player_name_and_rank.push_back(std::make_pair(player.get_name(), player_hand));
        }

        std::sort(player_name_and_rank.begin(), player_name_and_rank.end(), compare_hand_ranks);

        std::pair<Poker_Ranks, int> highest_rank = player_name_and_rank[0].second;

        std::vector<std::string> winners;
        for (const auto &player : player_name_and_rank) {
            
            if (player.second == highest_rank) {
                winners.push_back(player.first);
            } else {
            break;
        }
}

        if (has_run == false){
            if (winners.size() == 1) {
                std::cout << "The winner is " << winners[0] << " with a hand rank of " << poker_rank_to_string(highest_rank.first)<<" of highest card rank "<<highest_rank.second<< std::endl;
            } else {
                std::cout << "There are multiple winners with a hand rank of " << poker_rank_to_string(highest_rank.first)<<" of highest card rank "<< highest_rank.second << std::endl;
                for (const auto &winner : winners) {
                    std::cout << winner << std::endl;
                }
            }
            has_run=true;
        }
        return winners;
    }


    
    
    
    std::pair<Poker_Ranks, int> evaluate_hand (Card card1, Card card2, std::vector<Card> community_cards) {
        std::map<std::string, int> rank_to_num = {{"2", 2}, {"3", 3}, {"4", 4}, {"5", 5}, {"6", 6}, {"7", 7}, {"8", 8}, {"9", 9}, {"10", 10}, {"J", 11}, {"Q", 12}, {"K", 13}, {"A", 14}};
        std::vector<int> numeric_ranks;
        std::vector<std::string> suits;
        int highest_card{0};
    
    
        numeric_ranks.push_back(rank_to_num[card1.get_rank()]);
        numeric_ranks.push_back(rank_to_num[card2.get_rank()]);
    
        suits.push_back(card1.get_suit());
        suits.push_back(card2.get_suit());
    
        for (auto object : community_cards) {
            numeric_ranks.push_back(rank_to_num[object.get_rank()]);
            suits.push_back(object.get_suit());
        }
    
        std::sort(numeric_ranks.begin(), numeric_ranks.end());

        //initialise map for rank counts and suit counts
        std::map<int, int> rank_counts;
                
        for(auto object : numeric_ranks) {
            ++rank_counts[object];
        }
          //do the same for suits
        std::map<std::string, int> suit_counts;
        
        for(auto object : suits) {
            ++suit_counts[object];
        }
        
        int two_pair{0};
        int three_of_a_kind{0};
        int flush{0};
        int straight{0};

        std::vector<Poker_Ranks> hand_ranking;
        
        auto add_hand_ranking = [&hand_ranking] (Poker_Ranks rank) {hand_ranking.push_back(rank);};
        
        add_hand_ranking(Poker_Ranks::high_card);


        for(auto entry : rank_counts) {
            if(two_pair ==0){
                highest_card = std::max(highest_card, entry.first);
            }
            if(entry.second == 2) {
                two_pair += 1;
                add_hand_ranking(Poker_Ranks::pair);
                highest_card = entry.first;
            } 
        }
        
        if(two_pair >= 2) {
            add_hand_ranking(Poker_Ranks::two_pair);
        }
            
        for(auto entry : rank_counts) {
            if (entry.second == 3) {
                three_of_a_kind += 1;
                add_hand_ranking(Poker_Ranks::three_of_a_kind);
                highest_card = entry.first;
            }
        }
        
        for(int i = 0; i < numeric_ranks.size()-4; i++) {
            if (numeric_ranks[i+1] == numeric_ranks[i]+1 && numeric_ranks[i+2] == numeric_ranks[i]+2 && numeric_ranks[i+3] == numeric_ranks[i]+3 && numeric_ranks[i+4] == numeric_ranks[i]+4) {
                add_hand_ranking(Poker_Ranks::straight);
                highest_card = numeric_ranks[i+4];
                break;
            }
        }
        
        
    

        for(auto entry : suit_counts) {
            if(entry.second > 4) {
                
                add_hand_ranking(Poker_Ranks::flush);
            }
        }
        
        if((two_pair > 0 && three_of_a_kind > 0) || (three_of_a_kind ==2 )) {
           
            add_hand_ranking(Poker_Ranks::full_house);
        }
        
        
        for (auto entry : rank_counts){
            if(entry.second == 4){
                add_hand_ranking(Poker_Ranks::four_of_a_kind);
                highest_card = entry.first;
            }
        }
        
        if (straight == 1 && flush == 1){
            add_hand_ranking(Poker_Ranks::straight_flush);
            
            int royal_flush_elements{0};
            for (auto entry : rank_counts){
                if (entry.first == 10 || entry.first == 11 || entry.first == 12 || entry.first == 13 || entry.first == 14){
                    royal_flush_elements +=1;
                    
                }
            }
            if (royal_flush_elements == 5){
                add_hand_ranking(Poker_Ranks::royal_flush);
                highest_card = 14;
            }
        }
        
 
        Poker_Ranks highest_rank = hand_ranking.back();
        
        return {highest_rank, highest_card};
    } // evaluate card function end
    
    
    static std::string poker_rank_to_string(Poker_Ranks rank) {
    switch (rank) {
        case Poker_Ranks::high_card:       return "High Card";
        case Poker_Ranks::pair:            return "Pair";
        case Poker_Ranks::two_pair:        return "Two Pair";
        case Poker_Ranks::three_of_a_kind: return "Three of a Kind";
        case Poker_Ranks::straight:        return "Straight";
        case Poker_Ranks::flush:           return "Flush";
        case Poker_Ranks::full_house:      return "Full House";
        case Poker_Ranks::four_of_a_kind:  return "Four of a Kind";
        case Poker_Ranks::straight_flush:  return "Straight Flush";
        case Poker_Ranks::royal_flush:     return "Royal Flush";
        default:                           return "Unknown";
        }
    }
    
};


class Game {
private:
    int difficulty;
    int nr_of_bots;
    int starting_chips;
    Deck deck;
    Bot bot;
    Human human;
    Pot pot;
   

public:
    Game(int difficulty, int nr_bot, int start_chips)
        : difficulty(difficulty), nr_of_bots(nr_bot), starting_chips(start_chips), deck(nr_of_bots + 1, 5), bot(), human(starting_chips, deck), pot() {}
  
          
          
    std::vector<Player> bots_in_the_game;
    std::vector<Player> bot_initial_copy;
    std::vector<Player> all_players_initial_copy;
    
    bool human_in_the_game{true};
    bool it_is_the_first_game{true};
    
    

    
    void bot_turn() {
        std::vector<Player>::iterator it = bots_in_the_game.begin();
        bool did_bots_raise{false};
        
        while (it != bots_in_the_game.end()) {
            
    
            // random code that generates bot action
    
            // Create a random number generator and a distribution
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(1, 5);
    
            // Generate a random number between 1 and 4
            int bot_decision_random_number = dist(gen);
            //generate another random number used for "all in" bot raises
            
            
            
            // determine the bot action with if
            if (bot_decision_random_number <= 4) {
                // CHECK
                std::cout << "bot " << it->get_name() << " made this action: check" << std::endl;
                ++it;
    
            } else if (bot_decision_random_number == 5 && it->get_chips()>0) {
                // RAISE
                
                std::uniform_int_distribution<> dist2(5, 20); 
                int bot_raise_random_number = dist2(gen);
                
                
                int bot_betting_amount{};
                did_bots_raise = true;
                
                if (difficulty == 4){
                    bot_betting_amount = (bot_raise_random_number + 0.005*starting_chips*bot_raise_random_number)*3;
                } else {
                    bot_betting_amount = bot_raise_random_number + 0.005*starting_chips*bot_raise_random_number;
                }
                
                //introduces more bot raise decision options
                if (bot_raise_random_number == 19){
                    bot_betting_amount = bot_betting_amount*5;
                } else if (bot_raise_random_number ==20){
                    bot_betting_amount = bot_betting_amount*2;
                }
                
                if (bot_betting_amount > it->get_chips()){
                    bot_betting_amount = it->get_chips();
                    std::cout<<"bot "<<it->get_name()<<"made this action: raise "<< bot_betting_amount<< "  and went ALL IN!!!"<<std::endl;
                    it->bet(bot_betting_amount, pot);
                    bot.update_bot_chips(it-> get_name(), bot_betting_amount);
                } else{
                std::cout << "the bot named " << it->get_name() << " made this action: raise " <<bot_betting_amount<< std::endl;
                it->bet(bot_betting_amount, pot);
                bot.update_bot_chips(it-> get_name(), bot_betting_amount);
                }
    
             
    
                // Find the iterator pointing to the current bot in the other_bots_in_the_game vector
                 auto current_bot_it = std::find(bots_in_the_game.begin(), bots_in_the_game.end(), *it);

                bot_response(bot_betting_amount, it->get_name());

            

                if (human_in_the_game == true && human.get_chips()>0) {
                    human_response(bot_betting_amount);
                }
    
                break;
    
            } 
        }
            
    }


    
    void bot_response(int bet_amount, const std::string &raiser_bot_name) {
        std::vector<int> indices_to_remove;
        for (int i = 0; i < bots_in_the_game.size(); ++i) {
            if (bots_in_the_game[i].get_name() == raiser_bot_name) {
                continue;
            }
    
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(1, 3);
    
            int random_number = dist(gen);
    
            if (random_number == 1 && bots_in_the_game[i].get_chips()>0 &&bots_in_the_game.size()>0) {
                // FOLD
                std::cout << "the bot named " << bots_in_the_game[i].get_name() << " responded with this action: fold" << std::endl;
                indices_to_remove.push_back(i);
            } else if (bots_in_the_game[i].get_chips()>0){
                //CALL
                std::cout << "the bot named " << bots_in_the_game[i].get_name() << " responded with this action: call" << std::endl;
                if (bet_amount>=bots_in_the_game[i].get_chips()){
                    std::cout<<"Bot "<<bots_in_the_game[i].get_name()<<" went ALL IN !!!"<<std::endl;
                    int smaller_bet_amount = bots_in_the_game[i].get_chips();
                    bots_in_the_game[i].bet(smaller_bet_amount, pot);
                    bot.update_bot_chips(bots_in_the_game[i].get_name(), smaller_bet_amount);
                } else{
                    bots_in_the_game[i].bet(bet_amount, pot);
                    bot.update_bot_chips(bots_in_the_game[i].get_name(), bet_amount);
                }
            }
        }
        std::sort(indices_to_remove.begin(), indices_to_remove.end(), std::greater<>()); // sort in descending order
        for (int index : indices_to_remove) {
            bots_in_the_game.erase(bots_in_the_game.begin() + index);
        }
    }



    
    bool human_turn() {
        bool validInput = false;
        bool did_bots_respond = false;
        
        while (!validInput) {
            try {
                std::cout << "Choose one of the following actions: fold, check, raise" << std::endl;
                std::string user_action;
                std::cin >> user_action;
        
                if (user_action == "check") {
                    std::cout << "Human player checked" << std::endl;
                    validInput = true;
                } else if (user_action == "raise"){
                    int human_bet_amount;
                    int max_chips;
                    std::string max_chips_player_name;
                    
                    auto max_chips_player_it = std::max_element(bots_in_the_game.begin(), bots_in_the_game.end(), 
                    [](const Player &a, const Player &b) {
                        return a.get_chips() < b.get_chips();
                    });
                    max_chips = max_chips_player_it->get_chips();
                    max_chips_player_name = max_chips_player_it->get_name();
                    
                    std::cout << "You have " << human.get_chips() << " chips" <<std::endl<< "max opponent chips are: "<<max_chips<< " held by "<<max_chips_player_name<<std::endl<<"How much are you betting?" << std::endl;
                    std::cin >> human_bet_amount;
                    if (human_bet_amount <= 0 || human_bet_amount > human.get_chips() || human_bet_amount>max_chips) {
                        throw std::invalid_argument("Invalid bet amount!");
                    }
    
                    human.bet(human_bet_amount, pot);
                    std::string bot_raiser_name = "none";
                    bot_response(human_bet_amount, bot_raiser_name);
                    did_bots_respond = true;
                    validInput = true;
                } else if (user_action == "fold") {
                    std::cout << "Human player folded" << std::endl;
                    human_in_the_game = false;
                    validInput = true;
                } else {
                    throw std::invalid_argument("Invalid action. ");
                }
            } 
            catch (const std::invalid_argument& e) {
                std::cerr << e.what() << '\n';
            }
        }

    return did_bots_respond;
}

    
    
    
    void human_response(int amount){
        bool validInput = false;
        
        while (!validInput) {
            try {
                std::cout << "Other players are betting "<< amount << ", and you currently have: "<< human.get_chips() << ". What is your action?"<<std::endl<<"call, fold, raise"<<std::endl;
                std::string human_action;
                std::cin >> human_action;
    
                if (human_action == "fold") {
                    std::cout << "Human player folded"<<std::endl;
                    human_in_the_game = false;
                    validInput = true;
                } 
                else if (human_action == "call") {
                    int human_bet = amount;
                    std::cout << "Human player called"<<std::endl;
                    if (human.get_chips() < amount) {
                        human_bet = human.get_chips();
                        std::cout<<"human player went ALL IN!"<<std::endl;
                    }
                    
                    human.bet(human_bet, pot);
                    validInput = true;
                } 
                else if (human_action == "raise") {
                    if (human.get_chips() <= amount) {
                        throw std::invalid_argument("You don't have enough chips to raise!");
                    }
                    
                    int max_chips;
                    std::string max_chips_player_name;
                    
                    auto max_chips_player_it = std::max_element(bots_in_the_game.begin(), bots_in_the_game.end(), 
                    [](const Player &a, const Player &b) {
                        return a.get_chips() < b.get_chips();
                    });
                    max_chips = max_chips_player_it->get_chips();
                    max_chips_player_name = max_chips_player_it->get_name();
                    
                    std::cout << "How many extra chips are you adding?\n"<< " You currently have "<<human.get_chips()-amount<< " chips. Max opponent chips: "<< max_chips<<" held by "<<max_chips_player_name<<std::endl;
                    int extra_chips;
                    std::cin >> extra_chips;
                    if (extra_chips <= 0 || extra_chips > human.get_chips() - amount || extra_chips > max_chips) {
                        throw std::invalid_argument("Invalid number of extra chips!");
                    }
                    human.bet(amount, pot);
                    human.bet(extra_chips, pot);
                    std::string bot_raiser_name = "na";
                    bot_response(extra_chips, bot_raiser_name);
                    validInput = true;
                } 
                else {
                    throw std::invalid_argument("Invalid action. Please choose 'fold', 'call', or 'raise'.");
                }
            } 
            catch (const std::invalid_argument& e) {
                std::cerr << e.what() << '\n';
            }
        }
    }
    
        
        void show_board(){
            std::cout<<"the current community pot is: "<< pot.get_final_pot()<<std::endl;
            
                
            std::cout<<"bots left in the game:  "<<bots_in_the_game.size()<<std::endl;
            Player::show_player_info(bots_in_the_game);
            deck.print_community_cards();
            human.show_human_cards();
            std::cout<<"Your current chips: "<<human.get_chips()<<std::endl;
            
    }
    
    
    void distribute_pot(std::vector<std::string> winning_player_names, std::vector<Player> all_final_players, int total_pot_amount) {
        //if winners went all in and do not compete for all of the pot
        int distributed_chips{0};
        int distributed_chips_count{0};
        int amount_to_distribute = total_pot_amount / winning_player_names.size();
        std::string partial_pot_winner;
        //checks all of the winners who do not receive the total shared pot
        for (const auto& player_name : winning_player_names) {
            for(auto &bot_initial : bot_initial_copy){
                if (player_name == bot_initial.get_name()){
                    if (amount_to_distribute > bot_initial.get_chips()*(std::max(bot_initial_copy.size()/2 , all_final_players.size()))){
                        bot.update_chips_for_bot(player_name, bot_initial.get_chips()*(std::max(bot_initial_copy.size()/2 , all_final_players.size())));
                        std::cout<<"Bot player "<< player_name << " receives a partial pot: "<< bot_initial.get_chips()*(std::max(bot_initial_copy.size()/2 , all_final_players.size())) <<" chips"<<std::endl;
                        partial_pot_winner = player_name;
                        distributed_chips += bot_initial.get_chips()*(std::max(bot_initial_copy.size()/2 , all_final_players.size()));
                        distributed_chips_count +=1;
                        break;
                        }
                    }
                }
        }
        
        if(distributed_chips_count>0 && (all_final_players.size()-distributed_chips_count)>0){
            int remaining_chips = (total_pot_amount-distributed_chips)/(all_final_players.size()-distributed_chips_count);
            for (const auto& player : all_final_players){
                if (partial_pot_winner != player.get_name()){
                    
                    if (player.get_name() == human.get_name()) {
                    human.receive_pot_share(remaining_chips);
                    std::cout<<"Human player "<< human.get_name() << " gets back their chips "<< remaining_chips <<" chips"<<std::endl;
                } else {
                    bot.update_chips_for_bot(player.get_name(), remaining_chips);
                    std::cout<<"Non-winner bot player "<< player.get_name() << " gets back their chips "<< remaining_chips <<" chips"<<std::endl;
                    }
                    
                }
            }
        }
        else if (distributed_chips_count == 0) {
            
            for (const auto& player_name : winning_player_names){
            
                if (player_name == human.get_name()) {
                    human.receive_pot_share(amount_to_distribute);
                    std::cout<<"Human player "<< human.get_name() << " receives "<< amount_to_distribute <<" chips"<<std::endl;
                } else {
                    bot.update_chips_for_bot(player_name, amount_to_distribute);
                    std::cout<<"Bot player "<< player_name << " receives "<< amount_to_distribute <<" chips"<<std::endl;
                }
            }
            
        }
    }   


    std::pair<Player, Player> get_two_random_players(std::vector<Player>& players) {
        // Initialize a random number generator with a random device
        std::random_device rd;
        std::mt19937 rng(rd());
    
        // Create a uniform distribution that covers the range of the vector's indices
        std::uniform_int_distribution<int> dist(0, players.size() - 1);
    
        // Get two random indices. Use a set to ensure they are unique
        std::set<int> randomIndices;
        while (randomIndices.size() < 2) {
            randomIndices.insert(dist(rng));
        }
    
        // Get iterators to the two random players
        auto it = randomIndices.begin();
        Player& player1 = players[*it];
        ++it;
        Player& player2 = players[*it];
    
        // Return the two players
        return std::make_pair(player1, player2);
    }
    
    
    
    
    
    
    
    
    
    void game_analytics(std::vector<std::string> winner_names, std::vector<Player> remaining_players, Ranking&ranking){
        
        std::cout<<std::endl<<"Game analytics feedback to the user:"<<std::endl;
        std::vector<std::pair<std::string, std::pair<Poker_Ranks, int>>> player_name_and_rank;
    
        for (const auto& player : remaining_players) {
            Card card1 = player.get_card1();
            Card card2 = player.get_card2();
    
            std::pair<Poker_Ranks, int> player_hand = ranking.evaluate_hand(card1, card2, deck.get_community_cards());
    
            player_name_and_rank.push_back(std::make_pair(player.get_name(), player_hand));
        }
        
        std::sort(player_name_and_rank.begin(), player_name_and_rank.end(), Ranking::compare_hand_ranks);
    
        if(human_in_the_game == true){
            bool human_is_among_the_winners = false;
            
            std::pair<Poker_Ranks, int> human_hand = ranking.evaluate_hand(human.get_card1(), human.get_card2(), deck.get_community_cards());
                
                std::pair<Poker_Ranks, int> highest_rank = player_name_and_rank[0].second;
                
            
            for(auto &name : winner_names){
                if (name == "Human"){
                    //checks if human was the winner
                    int rank_difference = static_cast<int>(human_hand.first) - static_cast<int>(player_name_and_rank[1].second.first);
                    
                    if (pot.get_final_pot() < 0.5*human.get_chips()){
                        //checks if human could have raised more
                        //based on rank difference it tells whether the user`s` moves were risky 
                        if (rank_difference > 1){
                            std::cout<<"Great job! \n Next time consider making bigger bets when you are in a similar situation, because you had way better hand rank"<<std::endl;
                        } else {
                            std::cout<<"Great job! \n It was a well balanced risk-reward betting ratio"<<std::endl;
                        }
                        
                        
                    } else{
                        
                        if (rank_difference >= 1 || player_name_and_rank.size() == 1){
                            std::cout<<"Great job! Analytics show that you played a very good game!"<<std::endl;
                        } else if (static_cast<int>(highest_rank.first) >= 3 && player_name_and_rank.size()>1 && rank_difference == 0){
                            std::cout<<"It was a RISKY RAISE that you made \n Other players had the same poker hand rank, but the rank was a strong one \n Overall, such raising strategy is likely to be profitable in the long run !"<<std::endl;
                        } else if (rank_difference == 0) {
                            std::cout<<"It was a RISKY RAISE that you made \n Risk-reward ratio was not the best - you got a bit fortunate with the win \n There is a high chance that such strategy would not be profitable in the long run !"<<std::endl;
                        }
                        
                        
                        
                    }
                    human_is_among_the_winners = true;
                }
            }
    
            if (!human_is_among_the_winners) {
                // if human lost the game
                int rank_difference = static_cast<int>(player_name_and_rank[0].second.first) - static_cast<int>(human_hand.first);
    
                if (rank_difference > 1) {
                    std::cout << "The difference between your hand rank and the winning hand rank was more than 1. \n It was a VERY BAD MOVE!\n In a similar situation consider FOLD as early as possible" << std::endl;
                } else if (rank_difference == 1) {
                    std::cout << "The difference between your hand rank and the winning hand rank was 1 . \n Try to be more aware of other players possible hands next time \n Also, try to Fold early on when your hands are not strong." << std::endl;
                } else{
                    std::cout<<"You were UNLUCKY this game. Opponents had identical hand rank, but with higher quality, so they won the game. \n It was a good game and with a bit more luck next time, you would probably win it!"<<std::endl;
                    }    
                }
            }
        
    
        if (human_in_the_game == false){
            //if human folded in the game
            std::pair<Poker_Ranks, int> human_hand = ranking.evaluate_hand(human.get_card1(), human.get_card2(), deck.get_community_cards());
            player_name_and_rank.push_back(std::make_pair(human.get_name(), human_hand));
            std::sort(player_name_and_rank.begin(), player_name_and_rank.end(), Ranking::compare_hand_ranks);
    
            std::pair<Poker_Ranks, int> highest_rank = player_name_and_rank[0].second;
            std::vector<std::string> fictional_winners;
            
            for (auto &player : player_name_and_rank){
                if(player.second == highest_rank){
                    fictional_winners.push_back(player.first);
                }
            }
            
            bool human_would_have_been_the_winner = false;
            for (auto &name : fictional_winners){
                if (name == "Human"){
                    // if human would have been the winner if he did not fold
                    int rank_difference = static_cast<int>(human_hand.first) - static_cast<int>(player_name_and_rank[1].second.first);
                    
                    if (rank_difference >1){
                        std::cout<<"You would have been the winner... \n It seems that you got scared and ran out of the game when YOU HAD THE BEST CARDS!\n A VERY BAD FOLD decision! "<<std::endl;
                    } else if (rank_difference == 1){
                        std::cout<<"You would have been the winner... \n It would have been a strong win! Other players did not have your hand rank \n Next time you can be more confident with similar cards"<<std::endl;
                    } else {
                        std::cout<<"You would have been the winner... but it would be a close one \n You and bots had the same rank type, but your rank quality would have been better!\n Next time you can try to play more aggressively in similar scenarios! "<<std::endl;
                    }
                    
                    std::cout<<" Your rank would have been:\n Human FINAL RANK: "<< Ranking::poker_rank_to_string(human_hand.first)<<" of order "<<human_hand.second<<"\n the winner`s hand rank was: "<< Ranking::poker_rank_to_string(player_name_and_rank[1].second.first) << " of order "<< player_name_and_rank[1].second.second<<std::endl;
                    human_would_have_been_the_winner = true;
                    break;
                }
            }
    
            if (human_would_have_been_the_winner == false){
                if (human_hand.first == highest_rank.first){
                    std::cout<<" Your and winner hand ranks would have been the same, but the winner opponent had a better rank quality :) \n  GOOD FOLD decision ! And analytics admit that you were unlucky this game..."<<std::endl;
                } else {
                    std::cout<<" Winner`s hand ranks were better \n so it was GOOD DECISION to FOLD"<<std::endl;
                    
                }
    
                std::cout<<" Your rank would have been:\n Human FINAL RANK: "<< Ranking::poker_rank_to_string(human_hand.first)<< " of order "<< human_hand.second <<"\n the winner`s hand rank was: "<< Ranking::poker_rank_to_string(highest_rank.first) << " of order "<< highest_rank.second<<std::endl;
                
            }
        }
    } // game analytics function end




    
    
    
    
    
    
    void run() {
        
        if (it_is_the_first_game == true){
            //initialise the game when it is the first game
            int bot_chips;
            std::cout<<"new bots created"<<std::endl;
            
            if (difficulty ==1){
                bot_chips = starting_chips/2;
                bot.create_bots(deck, nr_of_bots, bot_chips);
            } else if (difficulty == 2) {
                bot_chips = starting_chips;
                bot.create_bots(deck, nr_of_bots, bot_chips);
            } else if (difficulty == 3) {
                bot_chips = starting_chips*2;
                bot.create_bots(deck, nr_of_bots, bot_chips);
            } else if (difficulty == 4){
                bot_chips = starting_chips*10;
                bot.create_bots(deck, nr_of_bots, bot_chips);
            }
            
            
        } else {
            // for later games only deck and player cards are updated
            deck.recreate();
            deck.populate_game_cards();
            
            bot.update_bots();
            bot.redistribute_bot_cards(deck);
            human.redistribute_human_cards(deck);
        }
        
        
        //initialise the primary game conditions
        //containers help to keep track of the game 
        human_in_the_game = true;
        
        bots_in_the_game = bot.get_bots();
        bot_initial_copy = bot.get_bots();
        all_players_initial_copy = bot.get_bots();
        all_players_initial_copy.push_back(human);
        //main game loop
        for (int round = 1; round <= 4; ++round) {
            std::cout<<std::endl<<std::endl << "Round " << round << " begins" << std::endl;
            bool bots_responded = false;
            
           
            if (round == 1){
                // big and small blinds
                int big_blind{starting_chips/10};
                int small_blind{big_blind/2};
                if (difficulty == 4){
                    big_blind = big_blind*3;
                    small_blind=small_blind*3;
                }
                std::pair<Player, Player> two_players = get_two_random_players(all_players_initial_copy);
                Player &player1 = two_players.first;
                Player &player2 = two_players.second;
                
                Player::show_player_info(bots_in_the_game);
         
                    if (player1.get_name() == "Human"){
                        std::cout<<"BIG BLIND: Human" <<std::endl;
                    } else {
                        std::cout<<"BIG BLIND: Bot"<< std::endl;
                        for (auto &object : bots_in_the_game){
                            if (object.get_name() == player1.get_name()){
                                object.bet(big_blind, pot);
                                bot.update_bot_chips(player1.get_name(), big_blind);
                            }
                        }
                    }
                    
                    if (player2.get_name() == "Human"){
                        std::cout<<"SMALL BLIND: Human" <<std::endl;
                    } else {
                        std::cout<<"SMALL BLIND: Bot"<< std::endl;
                    }
                    
                human.show_human_cards();
                std::cout<<"your private pot is: "<<human.get_chips()<<std::endl;
                
                bot_response(big_blind, player1.get_name());
                if (bots_in_the_game.size()>0){
                    if (human.get_chips()>big_blind){
                        if (player1.get_name() != "Human" && player2.get_name() != "Human"){
                            human_response(big_blind);
                        } else if (player2.get_name() == "Human"){
                            human.bet(small_blind, pot);
                            human_response(big_blind-small_blind);
                        }
                    } else {
                        human_response (human.get_chips());
                    }
                }
            }
            //shows the board and populates community cards
            else if (round == 2) {
                deck.take_flop();
                show_board();
            } else if (round == 3) {
                deck.take_turn();
                show_board();
            } else if (round == 4) {
                deck.take_river();
                show_board();
            }

            
            
            
            int highest_chips{0};
            for (auto &object : bots_in_the_game ){
                highest_chips = std::max(highest_chips, object.get_chips());
            }
            if (highest_chips == 0){
                deck.take_flop();
                deck.take_turn();
                deck.take_river();
                break;
            }
            
            if (human_in_the_game == true && human.get_chips()>0 && round !=1){
                bots_responded = human_turn();
            } if (bots_in_the_game.size()>0 && bots_responded == false && round !=1){
                bot_turn();
            } else if ((bots_in_the_game.size() == 0 && human_in_the_game == true) || (bots_in_the_game.size()==1 && human_in_the_game == false)){
                //ends the main game round loop if there is only a single player remaining in the game
                deck.take_flop();
                deck.take_turn();
                deck.take_river();
                break;
            }
            
        
        }//game round loop ends
        
        
        std::vector<Player> remaining_players;
        std::cout<<"the number of bots left was: "<< bots_in_the_game.size()<<std::endl;
        if (human_in_the_game == true){
            remaining_players.push_back(human);
        }
        
        std::cout<<"final community pot: "<<pot.get_final_pot()<<std::endl;
        std::cout<<"final community cards: ";
        deck.print_community_cards();

        
        if (bots_in_the_game.size()>0){
            for (const auto& bot : bots_in_the_game) {
            remaining_players.push_back(bot);
            }
        }
    
        
        // Determine the winner
        std::vector<std::string> winner_names;
        Ranking ranking;
        
        winner_names = ranking.determine_winner(remaining_players, deck);
        
       
        // distribute the pot among winners
        distribute_pot(winner_names, remaining_players, pot.get_final_pot());
        
        
        //introduce game analytics for human player
        
        game_analytics(winner_names, remaining_players, ranking);
        
        
        
        //delete bots that are out of the game
        bot.delete_defeated_bots();
        
       
        //reset pot
        pot.reset();
        
        //reset game cards
        
        deck.reset();
        
        Ranking::reset();   
        
        
        it_is_the_first_game = false;
        
        
   
    }//run function ending
    
   
    
    void play_multiple_games(int nr_of_games) {
        for (int game = 1; game <= nr_of_games; ++game) {
            
            if(human.get_chips() > 0) {
                if(bot.get_bots_number() > 0 || it_is_the_first_game == true){
                    //if all conditions are met, the game continues
                    std::cout << "\n \n \n The Poker Game number " << game << " begins!\n";
                    run();
                }
                if (bot.get_bots_number() == 0 && it_is_the_first_game == false) {
                    std::cout << "\nCONGRATULATIONS! You have successfully defeated all of the bot players! Throughout these poker games you increased your chips up to: " << human.get_chips() << std::endl;
                    break;
                }
            } else if (human.get_chips() == 0) {
                // when the user lost the game
                while (true) {
                    try {
                        std::cout << "\nYou have lost the game... It appears that you do not have any chips left. Now you can choose one out of two options:\n"
                                  << "buyback - get free chips and continue playing\n"
                                  << "quit - quits the program\n";
                        std::string human_input;
                        std::getline(std::cin, human_input); // Use getline to handle spaces in the input
                        if (human_input == "buyback"){
                            human.receive_pot_share(starting_chips);
                            game -= 1;
                            break;
                        } else if (human_input == "quit"){
                            while (true) {
                                try {
                                    std::cout << "Before you quit, would you like to see the previous games betting history [yes/no]? ";
                                    std::string user_response;
                                    std::getline(std::cin, user_response); // Use getline to handle spaces in the input
                                    if (user_response == "yes"){
                                        const std::vector<std::shared_ptr<Bet>>& bet_history = pot.get_bets();
                                        for (const auto& bet : bet_history) {
                                            std::cout << "Player " << bet->get_player_name() << " bet "
                                                      << bet->get_amount() << " chips." << std::endl;
                                        }
                                        return;
                                    } else if (user_response == "no"){
                                        return;
                                    } else {
                                        throw std::invalid_argument("Invalid response. Please answer 'yes' or 'no'.");
                                    }
                                } catch (const std::exception& e) {
                                    std::cout << e.what() << '\n';
                                    throw; // Re-throw the exception to be caught by the outer loop
                                }
                            }
                        } else {
                            throw std::invalid_argument("Invalid response. Please choose 'buyback' or 'quit'.");
                            }
                    } catch (const std::exception& e) {
                        std::cout << e.what() << '\n';
                        continue;
                        }
                }
    
            }   
        }
            // after the games have passed
            std::string user_response;
            while (true) {
                try {
                    std::cout<<  "The game has finished. Restart the program if you would like to play again \n Would you like to see betting history? [yes/no] \n";
                    std::cin >> user_response;
            
                    if (user_response == "yes") {
                        const std::vector<std::shared_ptr<Bet>>& bet_history = pot.get_bets();
                        for (const auto& bet : bet_history) {
                            std::cout << "Player " << bet->get_player_name() << " bet "
                                    << bet->get_amount() << " chips." << std::endl;
                        }
                        break;
                    } else if (user_response != "no") {
                        throw std::invalid_argument("Invalid response. Please answer 'yes' or 'no'.");
                    } else {
                        break;
                    }
                } catch (const std::invalid_argument& e) {
                    std::cout << e.what() << '\n';
                }
            }
     
    }

};

} //namespace Game end

bool Game::Ranking::has_run=false;


int main() {
    int bot_number;
    int nr_of_games;
    int starting_chips;
    int difficulty;
    
    //user enters the bot number and the input is checked
    while (true) {
        try {
            std::cout<<"Welcome to the poker game, enter the number of bots you will be playing against (5 recommended, 20 max): ";
            std::cin>>bot_number;

            if(std::cin.fail()){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("Please enter a valid integer.");
            }

            if (bot_number <= 0 || bot_number>20) {
                throw std::runtime_error("The number of bots should be a positive integer, smaller than 20.");
            }
            break;
        } catch (const std::exception& e) {
            std::cout << e.what() << '\n';
        }
    }
    //users enters the number of game and the input is checked
    while (true) {
        try {
            std::cout<<"Now enter the number of maximum poker games you are willing to play: ";
            std::cin>> nr_of_games;

            if(std::cin.fail()){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("Please enter a valid integer.");
            }

            if (nr_of_games <= 0) {
                throw std::runtime_error("The number of games should be a positive integer.");
            }
            break;
        } catch (const std::exception& e) {
            std::cout << e.what() << '\n';
        }
    }
    //user enters starting chips count and the input is checked
    while (true) {
        try {
            std::cout<<"Enter the starting individual pot (chips) number (100 - 200 recommended for best game experience): ";
            std::cin>>starting_chips;

            if(std::cin.fail()){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("Please enter a valid integer.");
            }

            if (starting_chips < 10 || starting_chips>10000) {
                throw std::runtime_error("Try to be more realistic :) \n allowed range is from 10 to 10000");
            }
            break;
        } catch (const std::exception& e) {
            std::cout << e.what() << '\n';
        }
    }
    //user enters the game difficulty integer and the input is checked
    while (true) {
        try {
            std::cout<<"Now choose the game difficulty level (1-4)\n 1 - Easy        ``Hey, how do we player poker again?`` \n 2 - Medium      ``I think I can handle this game`` \n 3 - Hard        ``I need a challenge because I am too good at poker`` \n 4 - Impossible  ``I am prepared to face my doom...`` \n  " ;
            std::cin>>difficulty;

            if(std::cin.fail()){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("Please enter a valid integer - 1, 2, 3 or 4.");
            }

            if (difficulty <= 0 || difficulty > 4) {
                throw std::runtime_error("There are only 4 difficulties :) \n allowed levels are 1, 2, 3 and 4");
            }
            break;
        } catch (const std::exception& e) {
            std::cout << e.what() << '\n';
        }
    }
    
    //main class is initialized
    Game::Game game(difficulty, bot_number, starting_chips);
    //game is started
    game.play_multiple_games(nr_of_games);

    return 0;
}