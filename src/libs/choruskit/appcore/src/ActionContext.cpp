#include "ActionContext.h"
#include "ActionContext_p.h"

#include "ActionItem.h"
#include "ActionSystem.h"

#include <QDebug>

class TreeNode {
public:
    QString id;
    TreeNode *parent;
    QMChronMap<QString, TreeNode *> children;

    TreeNode(const QString &id, TreeNode *parent, const QMChronMap<QString, TreeNode *> &children = {})
        : id(id), parent(parent), children(children) {
    }

    ~TreeNode() {
        qDeleteAll(children);
    }
};

namespace Core {

    ActionContextPrivate::ActionContextPrivate() : q_ptr(nullptr), configurable(true), stateDirty(false) {
    }

    void ActionContextPrivate::init() {
    }

    void ActionContextPrivate::setDirty() {
        stateDirty = true;
        emit q_ptr->stateChanged();
    }

    bool ActionContextData::isValid() const {
        return d != nullptr;
    }

    QString ActionContextData::id() const {
        return d ? d->id : QString();
    }

    bool ActionContextData::isGroup() const {
        return d ? d->isGroup : false;
    }

    QList<ActionInsertRule> ActionContextData::rules() const {
        return d ? d->rules : QList<ActionInsertRule>();
    }

    void ActionContextData::setRules(const QList<ActionInsertRule> &rules) {
        if (!d)
            return;
        d->rules = rules;

        d->d->setDirty();
        emit d->d->q_ptr->actionRulesChanged(d->id, rules);
    }

    QList<QKeySequence> ActionContextData::shortcuts() const {
        return d ? d->shortcuts : QList<QKeySequence>();
    }

    void ActionContextData::setShortcuts(const QList<QKeySequence> &shortcuts) {
        if (!d)
            return;
        d->shortcuts = shortcuts;
        emit d->d->q_ptr->actionShortcutsChanged(d->id, shortcuts);
    }

    ActionContextData::ActionContextData(ActionContextDataPrivate *d) : d(d) {
    }

    ActionContext::ActionContext(const QString &id, QObject *parent)
        : ActionContext(*new ActionContextPrivate(), id, parent) {
        Q_D(ActionContext);
        d->title = id;
    }

    ActionContext::ActionContext(const QString &id, const QString &title, QObject *parent)
        : ActionContext(*new ActionContextPrivate(), id, parent) {
        Q_D(ActionContext);
        d->title = title;
    }

    ActionContext::~ActionContext() {
        Q_D(ActionContext);
    }

    QString ActionContext::id() const {
        Q_D(const ActionContext);
        return d->id;
    }

    QString ActionContext::title() const {
        Q_D(const ActionContext);
        return d->title;
    }

    void ActionContext::setTitle(const QString &title) {
        Q_D(ActionContext);
        d->title = title;
        emit titleChanged(title);
    }

    ActionContextData ActionContext::addAction(const QString &id, bool isGroup) {
        Q_D(ActionContext);
        if (d->actions.contains(id)) {
            qWarning() << "Core::ActionContext::addAction(): trying to add duplicated action:" << id;
            return ActionContextData(nullptr);
        }
        auto it = d->actions.append(id, {d, id, isGroup, {}, {}});

        d->setDirty();
        emit actionAdded(id);

        return ActionContextData(&(it.value()));
    }

    void ActionContext::removeAction(const QString &id) {
        Q_D(ActionContext);
        auto it = d->actions.find(id);
        if (it == d->actions.end()) {
            qWarning() << "Core::ActionContext::removeAction(): action does not exist:" << id;
            return;
        }
        d->actions.erase(it);

        d->setDirty();
        emit actionRemoved(id);
    }

    ActionContextData ActionContext::action(const QString &id) {
        Q_D(ActionContext);
        auto it = d->actions.find(id);
        if (it == d->actions.end()) {
            return ActionContextData(nullptr);
        }
        return ActionContextData(&(it.value()));
    }

    QStringList ActionContext::actionIds() const {
        Q_D(const ActionContext);
        return d->actions.keys();
    }

    bool ActionContext::configurable() const {
        Q_D(const ActionContext);
        return d->configurable;
    }

    void ActionContext::setConfigurable(bool configurable) {
        Q_D(ActionContext);
        d->configurable = configurable;
    }

    QMap<QString, QStringList> ActionContext::state() const {
        auto system = qobject_cast<ActionSystem *>(parent());
        if (!system) {
            return {};
        }

        Q_D(const ActionContext);
        if (!d->stateDirty) {
            return d->state;
        }

        TreeNode *root = nullptr;
        {
            QHash<QString, TreeNode *> nodes;
            QSet<TreeNode *> isolatedNodes;

            // Filter cache
            QMap<QString, QStringList> stateCache = system->stateCache(d->id);
            for (auto it = stateCache.begin(); it != stateCache.end(); ++it) {
                const auto &id = it.key();

                TreeNode *node;
                if (id.isEmpty()) {
                    node = root = new TreeNode({}, nullptr);
                } else {
                    // Find definition
                    auto it2 = d->actions.find(id);
                    if (it2 == d->actions.end() || !it2->isGroup) {
                        continue;
                    }

                    auto it3 = nodes.find(id);
                    if (it3 == nodes.end()) {
                        node = new TreeNode{id, nullptr};
                        nodes.insert(id, node);
                        isolatedNodes.insert(node);
                    } else {
                        node = it3.value();
                    }
                }

                for (const auto &childId : qAsConst(it.value())) {
                    // Find definition
                    auto it3 = d->actions.find(childId);
                    if (it3 == d->actions.end()) {
                        continue;
                    }

                    auto it4 = nodes.find(id);
                    if (it4 == nodes.end()) {
                        auto childNode = new TreeNode{childId, node};
                        node->children.append(id, childNode);
                        nodes.insert(childId, childNode);
                    } else {
                        auto &childNode = it4.value();
                        if (!childNode->parent) {
                            childNode->parent = node;
                            node->children.append(childId, node);
                            isolatedNodes.remove(childNode);
                        }
                    }
                }
            }

            qDeleteAll(isolatedNodes);
        }

        if (!root) {
            root = new TreeNode{{}, nullptr};
        }

        QHash<QString, TreeNode *> nodes;
        auto actions = d->actions;

        // Rebuild indexes
        {
            std::list<TreeNode *> stack;
            stack.push_back(root);
            while (!stack.empty()) {
                auto node = stack.front();
                stack.pop_front();

                nodes.insert(node->id, node);
                actions.remove(node->id);

                for (const auto &child : qAsConst(node->children)) {
                    stack.push_back(child);
                }
            }
        }

        int sz;
        do {
            sz = actions.size();
            for (auto it = actions.begin(); it != actions.end();) {
                const auto &id = it->id;
                const auto &rules = it->rules;

                // Insert to root
                if (rules.isEmpty()) {
                    auto node = new TreeNode{id, root};
                    root->children.append(id, node);
                    nodes.insert(id, node);
                    goto done;
                }

                for (const auto &rule : rules) {
                    TreeNode *target;

                    if (rule.id.isEmpty()) {
                        target = root;
                    } else {
                        // Get target definition
                        auto it2 = d->actions.find(rule.id);
                        if (it2 == d->actions.end()) {
                            continue;
                        }

                        auto it3 = nodes.find(rule.id);
                        if (it3 == nodes.end()) {
                            continue;
                        }

                        if (it2->isGroup) {
                            target = it3.value();
                        } else {
                            target = it3.value()->parent;
                            goto non_group;
                        }
                    }

                    switch (rule.direction) {
                        case ActionInsertRule::Append: {
                            auto node = new TreeNode{id, target};
                            target->children.append(id, node);
                            nodes.insert(id, node);
                            goto done;
                            break;
                        }
                        case ActionInsertRule::Unshift: {
                            auto node = new TreeNode{id, target};
                            target->children.prepend(id, node);
                            nodes.insert(id, node);
                            goto done;
                            break;
                        }
                        default:
                            break;
                    }

                    continue;

                non_group:
                    switch (rule.direction) {
                        case ActionInsertRule::Append: {
                            auto node = new TreeNode{id, target};
                            target->children.insert(std::next(target->children.find(rule.id)), id, node);
                            nodes.insert(id, node);
                            goto done;
                            break;
                        }
                        case ActionInsertRule::Unshift: {
                            auto node = new TreeNode{id, target};
                            target->children.insert(target->children.find(rule.id), id, node);
                            nodes.insert(id, node);
                            goto done;
                            break;
                        }
                        default:
                            break;
                    }
                }

                ++it;
                continue;

            done:
                it = actions.erase(it);
            }

        } while (sz != actions.size());

        // Serialize tree
        QMap<QString, QStringList> res;
        {
            QStringList children;
            for (const auto &child : qAsConst(root->children)) {
                children << child->id;
            }
            res.insert(QString(), children);
        }

        for (const auto &node : qAsConst(nodes)) {
            if (node->children.isEmpty()) {
                continue;
            }

            QStringList children;
            for (const auto &child : qAsConst(node->children)) {
                children << child->id;
            }
            res.insert(node->id, children);
        }

        d->state = res;
        d->stateDirty = false;

        delete root;

        return res;
    }

    static void insertActionGroup(QActionGroup *group, ActionItem *item) {
        switch (item->type()) {
            case ActionItem::Menu: {
                group->addAction(item->menu()->menuAction());
                break;
            }
            case ActionItem::ActionGroup: {
                for (const auto &action : item->actionGroup()->actions()) {
                    group->addAction(action);
                }
                break;
            }
            case ActionItem::Action: {
                group->addAction(item->action());
                break;
            }
            default:
                break;
        }
    }

    static void insertMenuBar(QMenuBar *menuBar, ActionItem *item) {
        switch (item->type()) {
            case ActionItem::Menu: {
                menuBar->addMenu(item->menu());
                break;
            }
            case ActionItem::Action: {
                menuBar->addAction(item->action());
                break;
            }
            default:
                break;
        }
    }

    static void insertMenu(QMenu *menu, ActionItem *item) {
        auto sep = [&]() {
            if (!menu->isEmpty()) {
                menu->addSeparator();
            }
        };

        switch (item->type()) {
            case ActionItem::Menu:
                sep();
                menu->addMenu(item->menu());
                break;
            case ActionItem::ActionGroup: {
                sep();
                for (const auto &action : item->actionGroup()->actions()) {
                    menu->addAction(action);
                }
                break;
            }
            case ActionItem::Action: {
                sep();
                menu->addAction(item->action());
                break;
            }
            default:
                break;
        }
    }

    template <class Menu>
    static void buildMenu(const QList<ActionItem *> &items, Menu *menuBar, const QMap<QString, QStringList> &state,
                          void(insertion)(Menu *, ActionItem *)) {
        if (!menuBar)
            return;
        menuBar->clear();

        QHash<QString, ActionItem *> itemMap;

        // Clear items and build map
        for (const auto item : items) {
            itemMap.insert(item->id(), item);
        }

        // Fill action group first
        for (auto it = state.begin(); it != state.end(); ++it) {
            const auto &id = it.key();
            const auto &ids = it.value();
            if (ids.isEmpty()) {
                continue;
            }

            if (id.isEmpty()) {
                continue;
            } else {
                auto item = itemMap.value(id, nullptr);
                if (!item) {
                    continue;
                }
                switch (item->type()) {
                    case ActionItem::ActionGroup: {
                        auto actionGroup = item->actionGroup();

                        for (const auto &action : actionGroup->actions()) {
                            actionGroup->removeAction(action);
                        }

                        for (const auto &childId : qAsConst(ids)) {
                            auto childItem = itemMap.value(childId, nullptr);
                            if (!childItem)
                                continue;
                            insertActionGroup(actionGroup, childItem);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        for (auto it = state.begin(); it != state.end(); ++it) {
            const auto &id = it.key();
            const auto &ids = it.value();
            if (ids.isEmpty()) {
                continue;
            }

            if (id.isEmpty()) {
                for (const auto &childId : qAsConst(ids)) {
                    auto childItem = itemMap.value(childId, nullptr);
                    if (!childItem)
                        continue;
                    insertion(menuBar, childItem);
                }
            } else {
                auto item = itemMap.value(id, nullptr);
                if (!item) {
                    continue;
                }
                switch (item->type()) {
                    case ActionItem::Menu: {
                        auto menu = item->menu();
                        menu->clear();
                        for (const auto &childId : qAsConst(ids)) {
                            auto childItem = itemMap.value(childId, nullptr);
                            if (!childItem)
                                continue;
                            insertMenu(menu, childItem);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }

    void ActionContext::buildMenuBarWithState(const QList<ActionItem *> &items, QMenuBar *menuBar) const {
        buildMenu(items, menuBar, state(), insertMenuBar);
    }

    void ActionContext::buildMenuWithState(const QList<ActionItem *> &items, QMenu *menu) const {
        buildMenu(items, menu, state(), insertMenu);
    }

    ActionContext::ActionContext(ActionContextPrivate &d, const QString &id, QObject *parent)
        : QObject(parent), d_ptr(&d) {
        d.q_ptr = this;
        d.init();

        d.id = id;
    }
}